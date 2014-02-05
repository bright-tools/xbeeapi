/** 

Copyright 2014 John Bailey
   
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

#include "XBeeDevice.hpp"
#include "XBeeApiCfg.hpp"

/** Number of bytes we need to 'peek' into the receive buffer in order to retrieve the 
    payload length */
#define INITIAL_PEEK_LEN (3U)
    
/** Enum of bytes with a special meaning when communicating with the XBee in API
    mode.  In escaped mode, these are the bytes that need to be escaped */
typedef enum
{
    XBEE_SB_XON             = 0x11,
    XBEE_SB_XOFF            = 0x13,
    XBEE_SB_FRAME_DELIMITER = 0x7E,
    XBEE_SB_ESCAPE          = 0x7D
} XBeeSerialSpecialBytes_e;

/** ASCII command to the XBee to request API mode 2 */
const char api_mode2_cmd[] = { 'A', 'T', 'A', 'P', ' ', '2', '\r' };

/** ASCII command to the XBee to request that it exit command mode */
const char exit_cmd_mode_cmd[] = { 'A', 'T', 'C', 'N', '\r' };

XBeeDevice::XBeeDevice( PinName p_tx, PinName p_rx, PinName p_rts, PinName p_cts ): m_if( p_tx, p_rx )
{
    m_escape = true;
    m_inAtCmdMode = false;
    m_rxMsgLastWasEsc = false;
    
    /* Can only do flow control on devices which support it */
#if defined ( DEVICE_SERIAL_FC )
    /* TODO: need rts and cts both set? */
    m_if.set_flow_control( mbed::SerialBase::Flow.RTSCTS, p_rts, p_cts );
#endif

    /* Attach RX call-back to the serial interface */
    m_if.attach( this, &XBeeDevice::if_rx, Serial::RxIrq); 
}

XBeeDevice::~XBeeDevice( void )
{
    /* Iterate all of the decoders and un-register them */
    for( FixedLengthList<XBeeApiFrameDecoder*, XBEEAPI_CONFIG_DECODER_LIST_SIZE>::iterator it = m_decoders.begin() ;
         it != m_decoders.end();
         ++it ) {
        (*it)->unregisterCallback();
    }
}

void XBeeDevice::if_rx( void )
{
    /* Keep going while there are bytes to be read */
    while(m_if.readable()) {
        
        uint8_t c = m_if.getc();
        
        /* Sanity check that if we're starting from an empty buffer the byte that we're
           receiving is a frame delimiter */
        if(( m_inAtCmdMode ) ||
           (( c == XBEE_SB_FRAME_DELIMITER ) ||
            ( m_rxBuff.getSize() ))) 
        {
            /* If it's an escape character we want to de-code the escape, so flag
               that we have a pending escape but don't modify the rx buffer */
            if( m_escape &&
               ( c == XBEE_SB_ESCAPE ))
            {
                m_rxMsgLastWasEsc = true;
            }
            else
            {
                if( m_rxMsgLastWasEsc ) {
                    c = c ^ 0x20;  
                    m_rxMsgLastWasEsc = false;
                }
                m_rxBuff.write( &c, 1 );
            }
        } else {
            /* TODO */    
        }
    }
    
    if( m_inAtCmdMode ) 
    {
        /* Safeguard - if we're in cmd mode, clear out status associated with API mode */
        m_rxMsgLastWasEsc = false;
    } 
    else 
    {
        /* Check to see if there's API data to decode */
        checkRxDecode();
    }
}
    
void XBeeDevice::checkRxDecode( void )
{
    uint8_t buff[INITIAL_PEEK_LEN];
    bool cont = false;
    
    /* Ensure that we're delimiter aligned - this should allow recovery in the case that
       we've missed bytes and somehow become unaligned */
    while( m_rxBuff.getSize() &&
          ( m_rxBuff[0] != XBEE_SB_FRAME_DELIMITER ))
    {
        m_rxBuff.chomp( 1 );
    }
    
    do {
        /* Get an initial portion of data from the read buffer so that the message length can be determined */
        uint16_t len = m_rxBuff.peek( buff, INITIAL_PEEK_LEN );    
        cont = false;
        
        /* Ensure that sufficient data was received - already know that we should be delimiter aligned based on the above */
        if( len >= INITIAL_PEEK_LEN ) 
        {
            /* Try and get enough data to cover the whole message */
            const uint16_t cmdLen = MSG_LEN_IN_BUFFER( buff ) + XBEE_API_FRAME_OVERHEAD;
            uint8_t cmdBuff[cmdLen];
            uint16_t len = m_rxBuff.peek( cmdBuff, cmdLen );    
    
            /* Check that we've received the entire frame */
            if( len >= cmdLen )
            {
                /* TODO: Verify checksum */
                
                /* Iterate all of the decoders */
                for( FixedLengthList<XBeeApiFrameDecoder*, XBEEAPI_CONFIG_DECODER_LIST_SIZE>::iterator it = m_decoders.begin() ;
                     it != m_decoders.end();
                     ++it ) {
    
                    bool processed = (*it)->decodeCallback( cmdBuff, cmdLen );
                    if( processed )
                    {
                        break;
                    }
                }            
                /* Remove the data from the receive buffer - either it was decoded (all well and good)
                   or it wasn't, in which case we need to get rid of it to prevent it from jamming
                   up the message queue */
                m_rxBuff.chomp( cmdLen );

                /* Successfully decoded 1 message ... there may be more waiting in the buffer! */                
                cont = true;
            }
        }
    } while( cont );
}

bool XBeeDevice::registerDecoder( XBeeApiFrameDecoder* const p_decoder )
{
    bool ret_val = false;
    if( p_decoder != NULL )
    {
        /* Check if decoder already registered */
        if( !m_decoders.inList( p_decoder ) ) 
        {
            m_decoders.push( p_decoder );   
            p_decoder->registerCallback( this );
            ret_val = true;
        }
    }
    return ret_val;
}
     
bool XBeeDevice::unregisterDecoder( XBeeApiFrameDecoder* const p_decoder )
{
    bool ret_val = false;
    if( p_decoder != NULL )
    {
        if( m_decoders.remove( p_decoder ) )
        {
            p_decoder->unregisterCallback();
            ret_val = true;   
        }
    }
    return ret_val;
}

void XBeeDevice::SendFrame( XBeeApiFrame* const p_cmd )
{
    uint8_t sum = 0U;
    uint16_t len;
    uint16_t i;
    const uint8_t* cmdData;
    uint16_t written = 0;
 
#if defined  XBEEAPI_CONFIG_USING_RTOS
    m_ifMutex.lock();
#endif

    xbeeWrite( XBEE_SB_FRAME_DELIMITER, false );
    
    len = p_cmd->getCmdLen();
    xbeeWrite((uint8_t)(len >> 8U));
    xbeeWrite((uint8_t)(len & 0xFF));

    sum += xbeeWrite((uint8_t)p_cmd->getApiId());
    len--;

    /* While data still to go out */
    while( written < len ) 
    {
        uint16_t buffer_len;     
        
        /* Get the next chunk of data from the frame object */
        p_cmd->getDataPtr( written, &cmdData, &buffer_len );

        /* Write the buffer to the XBee */
        for( i = 0;
             i < buffer_len;
             ++i,++written )
        {
            sum += xbeeWrite(cmdData[i]);
        }
    }
     
    /* Checksum is 0xFF - summation of bytes (excluding delimiter and length) */
    xbeeWrite( (uint8_t)0xFFU - sum );
    
    fflush( m_if );
#if defined XBEE_DEBUG_DEVICE_DUMP_MESSAGE_DECODE
    m_if.printf("\r\n");
#endif
    
#if defined  XBEEAPI_CONFIG_USING_RTOS
    m_ifMutex.unlock();
#endif
}

uint8_t XBeeDevice::xbeeWrite( uint8_t p_byte, bool p_doEscape )
{
    uint8_t c_sum = 0;
    
    if (p_doEscape && m_escape && 
        ((p_byte == XBEE_SB_FRAME_DELIMITER ) ||
         (p_byte == XBEE_SB_ESCAPE ) || 
         (p_byte == XBEE_SB_XON ) || 
         (p_byte == XBEE_SB_XOFF))) 
    {
#if defined XBEE_DEBUG_DEVICE_DUMP_MESSAGE_DECODE
        m_if.printf("%02x ",XBEE_SB_ESCAPE);
        m_if.printf("%02x ",p_byte ^ 0x20);
#else
        m_if.putc(XBEE_SB_ESCAPE);
        m_if.putc(p_byte ^ 0x20);
#endif
        c_sum += XBEE_SB_ESCAPE;
        c_sum += p_byte ^ 0x20;
    } else {
#if defined XBEE_DEBUG_DEVICE_DUMP_MESSAGE_DECODE
        m_if.printf("%02x ",p_byte);
#else
        m_if.putc(p_byte);
#endif
        c_sum += p_byte;
    }
    return c_sum;
}

#define IS_OK( _b ) (( _b[ 0 ] == 'O' ) && ( _b[ 1 ] == 'K' ) && ( _b[ 2 ] == '\r' ))
#define OK_LEN (3U)

XBeeDevice::XBeeDeviceReturn_t XBeeDevice::SendFrame( const char* const p_dat, size_t p_len, int p_wait_ms )
{
    XBeeDeviceReturn_t ret_val;

    if( m_inAtCmdMode )
    {
#if defined  XBEEAPI_CONFIG_USING_RTOS
        m_ifMutex.lock();
#endif
        for( size_t i = 0;
             i < p_len;
             i++ ) {
            m_if.putc(p_dat[i]);
        }
        
        fflush( m_if );
                
        wait_ms( p_wait_ms );
                
        /* Check the response for the OK indicator */
        if( m_rxBuff.getSize() == OK_LEN )
        {
            uint8_t ok_buff[OK_LEN];
            m_rxBuff.read( ok_buff, OK_LEN );
                    
            if( IS_OK( ok_buff ))
            {
                ret_val = XBEEDEVICE_OK;
            } 
            else 
            {
                ret_val = XBEEDEVICE_UNEXPECTED_DATA;                    
            }
        }
        else
        {
            ret_val = XBEEDEVICE_UNEXPECTED_LENGTH;
        }
#if defined  XBEEAPI_CONFIG_USING_RTOS
        m_ifMutex.unlock();
#endif
    } 
    else 
    {
        ret_val = XBEEDEVICE_WRONG_MODE;            
    }
    return ret_val;
}

XBeeDevice::XBeeDeviceReturn_t XBeeDevice::setUpApi( void )
{
    XBeeDeviceReturn_t ret_val;
    
    /* Wait for the guard period before transmitting command sequence */
    wait_ms( XBEEAPI_CONFIG_GUARDPERIOD_MS );
    
    m_inAtCmdMode = true;
    
    /* Request to enter command mode */
    /* TODO: Magic number */
    ret_val = SendFrame("+++", 3, 3000);

    /* Everything OK with last request? */
    if( ret_val == XBEEDEVICE_OK )
    {
        wait_ms( XBEEAPI_CONFIG_GUARDPERIOD_MS );
        
        /* API mode 2 please! */
        ret_val = SendFrame(api_mode2_cmd,sizeof(api_mode2_cmd));
    }

    /* Everything OK with last request? */
    if( ret_val == XBEEDEVICE_OK )
    {
        /* Exit command mode, back to API mode */
        ret_val = SendFrame(exit_cmd_mode_cmd,sizeof(exit_cmd_mode_cmd));
    }
    
    m_inAtCmdMode = false;
    
    return ret_val;
}

#if defined XBEEAPI_CONFIG_ENABLE_DEVELOPER

#define PRINTABLE_ASCII_FIRST 32U
#define PRINTABLE_ASCII_LAST 126U

void XBeeDevice::dumpRxBuffer( Stream* p_buf, const bool p_hexView )
{
    uint8_t c;
    while( m_rxBuff.getSize() ) {
        if( m_rxBuff.read( &c, 1 ) ) {
            if( p_hexView ) {
                uint8_t a = '-';
                if(( c>=PRINTABLE_ASCII_FIRST ) && (c<=PRINTABLE_ASCII_LAST)) {
                    a = c;
                }
                p_buf->printf("0x%02x (%c) ",c,a);
            } else {
                p_buf->printf("%c",c);
                if( c == '\r' ) {
                    p_buf->printf("\n");   
                }
            }
        }
    }
}

#endif