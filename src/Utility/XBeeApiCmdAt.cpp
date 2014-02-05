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

#include "XBeeApiCmdAt.hpp"

/* Set of Frame ID codes for the various commands (see XBEE_CMD_POSN_FRAME_ID) */

#define CMD_RESPONSE_GET_VR  '1'
#define CMD_RESPONSE_GET_HV  '2'
#define CMD_RESPONSE_GET_CH  '3'
#define CMD_RESPONSE_SET_CH  '4'
#define CMD_RESPONSE_SET_CE  '5'
#define CMD_RESPONSE_GET_CE  '6'
#define CMD_RESPONSE_SET_EDA '7'
#define CMD_RESPONSE_GET_EDA '8'
#define CMD_RESPONSE_SET_PID '9'
#define CMD_RESPONSE_GET_PID '0'

extern Serial pc;


/* Content for the various commands - value of 0 indicates a value to be populated (i.e. variable) */

const uint8_t cmd_vr[] =      { CMD_RESPONSE_GET_VR, 'V', 'R' };
const uint8_t cmd_hv[] =      { CMD_RESPONSE_GET_HV, 'H', 'V' };

const uint8_t cmd_ch[] =      { CMD_RESPONSE_GET_CH, 'C', 'H' };
const uint8_t cmd_set_ch[] =  { CMD_RESPONSE_SET_CH, 'C', 'H', 0 };

const uint8_t cmd_ce[] =      { CMD_RESPONSE_GET_CE, 'C', 'E' };
const uint8_t cmd_set_ce[] =  { CMD_RESPONSE_SET_CE, 'C', 'E', 0 };

const uint8_t cmd_eda[] =     { CMD_RESPONSE_GET_EDA, 'A', '1' };
const uint8_t cmd_set_eda[] = { CMD_RESPONSE_SET_EDA, 'A', '1', 0 };

const uint8_t cmd_pid[] =     { CMD_RESPONSE_GET_PID, 'I', 'D' };
const uint8_t cmd_set_pid[] = { CMD_RESPONSE_SET_PID, 'I', 'D', 0, 0 };

#define XBEE_CMD_POSN_FRAME_ID (4U)
#define XBEE_CMD_POSN_STATUS (7U)
#define XBEE_CMD_POSN_PARAM_START (8U)

#define XBEE_CMD_RESPONS_HAS_DATA( _p_len ) ((_p_len > ( XBEE_CMD_POSN_PARAM_START + 1 ))

XBeeApiCmdAt::XBeeApiCmdAt() : XBeeApiFrameDecoder( ) , m_haveHwVer( false ),
    m_haveFwVer( false ),
    m_haveChan( false ),
    m_havePANId( false ),
    m_haveEDA( false ),
    m_haveCE( false )
{
}

bool XBeeApiCmdAt::decodeCallback( const uint8_t* const p_data, size_t p_len )
{
    bool ret_val = false;

    if( XBEE_CMD_AT_RESPONSE == p_data[ XBEE_CMD_POSN_API_ID ] ) {

        switch( p_data[ XBEE_CMD_POSN_FRAME_ID ] ) {
            case CMD_RESPONSE_GET_HV:
                if( p_data[ XBEE_CMD_POSN_STATUS ] == 0 ) 
                {
                    m_hwVer = ((uint16_t)p_data[ XBEE_CMD_POSN_PARAM_START ] << 8) + p_data[ XBEE_CMD_POSN_PARAM_START + 1 ];
                    m_haveHwVer = true;
                } 
                else 
                {
                    pc.printf("\r\nERROR1\r\n");    
                }
                ret_val = true;
                break;

            case CMD_RESPONSE_GET_VR:
                if( p_data[ XBEE_CMD_POSN_STATUS ] == 0 ) 
                {
                    m_fwVer = ((uint16_t)p_data[ XBEE_CMD_POSN_PARAM_START ] << 8) + p_data[ XBEE_CMD_POSN_PARAM_START + 1 ];
                    m_haveFwVer = true;
                } 
                else 
                {
                    pc.printf("\r\nERROR2\r\n");    
                }
                ret_val = true;
                break;

            case CMD_RESPONSE_GET_CH:
            case CMD_RESPONSE_SET_CH:
                if( p_data[ XBEE_CMD_POSN_STATUS ] == 0 ) 
                {
                    if( CMD_RESPONSE_GET_CH == p_data[ XBEE_CMD_POSN_API_ID ] )
                    {
                        m_chan = p_data[ XBEE_CMD_POSN_PARAM_START ];
                    } 
                    else
                    {  
                        m_chan = m_chanPend;
                    }
                    #if 0
                    printf("\r\n%02x (%2d) - %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n",p_data[ XBEE_CMD_POSN_API_ID ],p_len,
                        p_data[0],p_data[1],p_data[2],p_data[3],p_data[4],p_data[5],p_data[6],p_data[7],p_data[8]);
                    #endif
                    m_haveChan = true;
                } 
                else 
                {
                    /* TODO */
                    pc.printf("\r\nERROR3\r\n");    
                }
                ret_val = true;
                break;

            case CMD_RESPONSE_SET_CE:
            case CMD_RESPONSE_GET_CE:
                if( p_data[ XBEE_CMD_POSN_STATUS ] == 0 ) 
                {
                    if( CMD_RESPONSE_GET_CE == p_data[ XBEE_CMD_POSN_API_ID ] )
                    {
                        m_CE = p_data[ XBEE_CMD_POSN_PARAM_START ];
                    }
                    else
                    {
                        m_CE = m_CEPend;
                    }
                    m_haveCE = true;
                } 
                else 
                {
                    pc.printf("\r\nERROR3\r\n");    
                }
                ret_val = true;
                break;

            case CMD_RESPONSE_SET_PID:
            case CMD_RESPONSE_GET_PID:
                if( p_data[ XBEE_CMD_POSN_STATUS ] == 0 ) 
                {
                    if( CMD_RESPONSE_GET_PID == p_data[ XBEE_CMD_POSN_API_ID ] )
                    {
                        m_PANId = p_data[ XBEE_CMD_POSN_PARAM_START ];
                    } 
                    else
                    {
                        m_PANId = m_PANIdPend;
                    }
                    m_havePANId = true;
                } 
                else 
                {
                    pc.printf("\r\nERROR3\r\n");    
                }
                ret_val = true;
                break;

            case CMD_RESPONSE_SET_EDA:
            case CMD_RESPONSE_GET_EDA:
                if( p_data[ XBEE_CMD_POSN_STATUS ] == 0 ) 
                {
                    if( CMD_RESPONSE_GET_EDA == p_data[ XBEE_CMD_POSN_API_ID ] )
                    {
                        m_EDA = p_data[ XBEE_CMD_POSN_PARAM_START ];
                    }
                    else
                    {
                        m_EDA = m_EDAPend;
                    }
                    m_haveEDA = true;
                } 
                else 
                {
                    pc.printf("\r\nERROR3\r\n");    
                }
                ret_val = true;
                break;

        }
    }
    return ret_val;
}

bool XBeeApiCmdAt::setChannel( uint8_t const p_chan )
{
    XBeeApiCmdAtSet<uint8_t> req( cmd_set_ch, p_chan );

    m_chanPend = p_chan;
    m_device->SendFrame( &req );
    return true;
}

bool XBeeApiCmdAt::requestHardwareVersion( void )
{
    XBeeApiFrame req( XBEE_CMD_AT_CMD, cmd_hv, sizeof( cmd_hv ));
    m_haveHwVer = false;
    m_device->SendFrame( &req );
    return true;
}

bool XBeeApiCmdAt::requestFirmwareVersion( void )
{
    XBeeApiFrame req( XBEE_CMD_AT_CMD, cmd_vr, sizeof( cmd_vr ));
    m_haveFwVer = false;
    m_device->SendFrame( &req );
    return true;
}

bool XBeeApiCmdAt::requestChannel( void )
{
    XBeeApiFrame req( XBEE_CMD_AT_CMD, cmd_ch, sizeof( cmd_ch ));
    m_haveChan = false;
    m_device->SendFrame( &req );
    return true;
}

bool XBeeApiCmdAt::requestPanId( void )
{
    XBeeApiFrame req( XBEE_CMD_AT_CMD, cmd_pid, sizeof( cmd_pid ));
    m_havePANId = false;
    m_device->SendFrame( &req );
    return true;
}

bool XBeeApiCmdAt::requestCoordinatorEnabled( void )
{
    XBeeApiFrame req( XBEE_CMD_AT_CMD, cmd_ce, sizeof( cmd_ce ));
    m_haveCE = false;
    m_device->SendFrame( &req );
    return true;
}

bool XBeeApiCmdAt::requestEndDeviceAssociationEnabled( void )
{
    XBeeApiFrame req( XBEE_CMD_AT_CMD, cmd_eda, sizeof( cmd_eda ));
    m_haveEDA = false;
    m_device->SendFrame( &req );
    return true;
}

bool XBeeApiCmdAt::getFirmwareVersion( uint16_t* const p_ver )
{
    if( m_haveFwVer ) {
        *p_ver = m_fwVer;
    }
    return m_haveFwVer;

}

bool XBeeApiCmdAt::getHardwareVersion( uint16_t* const p_ver )
{
    if( m_haveHwVer ) {
        *p_ver = m_hwVer;
    }
    return m_haveHwVer;
}

bool XBeeApiCmdAt::getChannel( uint8_t* const p_chan )
{
    if( m_haveChan ) {
        *p_chan = m_chan;
    }
    return m_haveChan;
}

bool XBeeApiCmdAt::getCoordinatorEnabled( bool* const p_en )
{
    if( m_haveCE ) {
        *p_en = m_CE;
    }
    return m_haveCE;
}

bool XBeeApiCmdAt::setCoordinatorEnabled( const bool p_en )
{
    XBeeApiCmdAtSet<bool> req( cmd_set_ce, p_en );

    m_haveCE = false;
    m_CEPend = p_en;
    m_device->SendFrame( &req );
    return true;
}

bool XBeeApiCmdAt::getEndDeviceAssociationEnabled( bool* const p_en )
{
    if( m_haveEDA ) {
        *p_en = m_EDA;
    }
    return m_haveEDA;
}
bool XBeeApiCmdAt::setEndDeviceAssociationEnabled( const bool p_en  )
{
    XBeeApiCmdAtSet<bool> req( cmd_set_eda, p_en );

    m_haveEDA = false;
    m_EDAPend = p_en;
    m_device->SendFrame( &req );
    return true;
}

bool XBeeApiCmdAt::getPanId( panId_t* const p_chan )
{
    if( m_havePANId ) {
        *p_chan = m_PANId;
    }
    return m_havePANId;

}
bool XBeeApiCmdAt::setPanId( const panId_t p_chan )
{
    XBeeApiCmdAtSet<panId_t> panid( cmd_set_pid, p_chan );

    m_havePANId = false;
    m_PANIdPend = p_chan;
    m_device->SendFrame( &panid );
    return true;
}


XBeeApiCmdAtBlocking::XBeeApiCmdAtBlocking( const uint16_t p_timeout, const uint16_t p_slice ) :
    XBeeApiCmdAt( ),
    m_timeout( p_timeout ),
    m_slice( p_slice )
{
}

/**
   Macro to wrap around the "requestXXX" & "getXXX" methods and implement a blocking call.
   This macro is used as the basis for getXXX functions in XBeeApiCmdAtBlocking.

   Originally looked to achieve this using a template function passing method pointers, however
   there's no way to get a method pointer to the parent class implementation as opposed to the
   implementation in this class, meaning that the result was a recursive method call.  The joys of
   polymorphism.

   e.g. We pass a pointer to method getHardwareVersion().  The function receiving the pointer
        uses it to make a function call.  The actual function that's called is (correctly)
        the one implemented in this class, however what we actually wanted in this case
        was to call the implementation in the base class.  Using static_cast<> doesn't have
        any effect and taking the address of XBeeApiCmdAt::getHardwareVersion ends up with
        XBeeApiCmdAtBlocking::getHardwareVersion being called due to polymorphism. */
#define BLOCKING_GET( _REQ_FN, _GET_FN, _VAR ) \
    bool ret_val = false; \
    uint16_t counter = m_timeout; \
\
    if( _GET_FN( _VAR ) )\
    {\
        ret_val = true;\
    } \
    else if( _REQ_FN() )\
    {\
        bool cont = false;\
\
        do{\
            wait_ms( m_slice );\
            if( _GET_FN( _VAR ) )\
            {\
                ret_val = true;\
            }\
            else if( counter > m_slice ) {\
                counter -= m_slice;    \
                cont = true;\
            } \
        } while( cont );\
    }\
    \
    return( ret_val );

/**
   Macro to wrap around the "setXXX" & "getXXX" methods and implement a blocking call.
   This macro is used as the basis for setXXX functions in XBeeApiCmdAtBlocking.
*/
#define BLOCKING_SET( _SET_FN, _GET_FN, _VAR, _TYPE ) \
    bool ret_val = false; \
    uint16_t counter = m_timeout; \
    _TYPE readback; \
\
    if( _SET_FN( _VAR ) )\
    {\
        bool cont = false;\
\
        do{\
            wait_ms( m_slice );\
            if( _GET_FN( &readback ) &&\
               ( readback == _VAR ))\
            {\
                ret_val = true;\
            }\
            else if( counter > m_slice ) {\
                counter -= m_slice;    \
                cont = true;\
            } \
        } while( cont );\
    }\
    \
    return( ret_val );


bool XBeeApiCmdAtBlocking::getHardwareVersion( uint16_t* const p_ver )
{
    BLOCKING_GET( XBeeApiCmdAt::requestHardwareVersion,
                  XBeeApiCmdAt::getHardwareVersion,
                  p_ver );
}

bool XBeeApiCmdAtBlocking::getFirmwareVersion( uint16_t* const p_ver )
{
    BLOCKING_GET( XBeeApiCmdAt::requestFirmwareVersion,
                  XBeeApiCmdAt::getFirmwareVersion,
                  p_ver );
}

bool XBeeApiCmdAtBlocking::getChannel( uint8_t* const p_chan )
{
    BLOCKING_GET( XBeeApiCmdAt::requestChannel,
                  XBeeApiCmdAt::getChannel,
                  p_chan );
}

bool XBeeApiCmdAtBlocking::setChannel( uint8_t const p_chan )
{
    BLOCKING_SET( XBeeApiCmdAt::setChannel,
                  XBeeApiCmdAt::getChannel,
                  p_chan,
                  uint8_t );
}

bool XBeeApiCmdAtBlocking::getCoordinatorEnabled( bool* const p_en )
{
    BLOCKING_GET( XBeeApiCmdAt::requestCoordinatorEnabled,
                  XBeeApiCmdAt::getCoordinatorEnabled,
                  p_en );
}

bool XBeeApiCmdAtBlocking::setCoordinatorEnabled( const bool p_en )
{
    BLOCKING_SET( XBeeApiCmdAt::setCoordinatorEnabled,
                  XBeeApiCmdAt::getCoordinatorEnabled,
                  p_en,
                  bool );
}

bool XBeeApiCmdAtBlocking::getEndDeviceAssociationEnabled( bool* const p_en )
{
    BLOCKING_GET( XBeeApiCmdAt::requestEndDeviceAssociationEnabled,
                  XBeeApiCmdAt::getEndDeviceAssociationEnabled,
                  p_en );
}

bool XBeeApiCmdAtBlocking::setEndDeviceAssociationEnabled( const bool p_en )
{
    BLOCKING_SET( XBeeApiCmdAt::setEndDeviceAssociationEnabled,
                  XBeeApiCmdAt::getEndDeviceAssociationEnabled,
                  p_en,
                  bool );
}

bool XBeeApiCmdAtBlocking::getPanId( panId_t* const p_chan )
{
    BLOCKING_GET( XBeeApiCmdAt::requestPanId,
                  XBeeApiCmdAt::getPanId,
                  p_chan );
}

bool XBeeApiCmdAtBlocking::setPanId( const panId_t p_chan )
{
    BLOCKING_SET( XBeeApiCmdAt::setPanId,
                  XBeeApiCmdAt::getPanId,
                  p_chan,
                  panId_t );
}

template < typename T >
XBeeApiCmdAt::XBeeApiCmdAtSet<T>::XBeeApiCmdAtSet( const uint8_t* const p_data,
                                                   const T p_val ) : XBeeApiFrame( )
{
    size_t s;
    uint8_t* dest;
    const uint8_t* src = (uint8_t*)(&p_val);
    
    m_apiId = XBEE_CMD_AT_CMD;
    
    m_buffer[0] = p_data[0];
    m_buffer[1] = p_data[1];
    m_buffer[2] = p_data[2];
    
    dest = &( m_buffer[3] );
    
    for( s = 0;
         s < sizeof( T );
         s++, dest++, src++ ) {
        *dest = *src;         
    }
    
    m_data = m_buffer;
    m_dataLen = sizeof( m_buffer );
}

template < typename T >
XBeeApiCmdAt::XBeeApiCmdAtSet<T>::~XBeeApiCmdAtSet()
{
}