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

#include "XBeeApiTxFrame.hpp"

XBeeApiTxFrame::XBeeApiTxFrame( XBeeDevice* p_device ) : XBeeApiFrame(), XBeeApiFrameDecoder( p_device ),
                                                         m_addr( XBEE_BROADCAST_ADDR ),
                                                         m_ack( true ), 
                                                         m_panBroadcast( false )
{
    m_apiId = XBEE_CMD_TX_16B_ADDR;
}


XBeeApiTxFrame::~XBeeApiTxFrame( void ) 
{
}

uint8_t XBeeApiTxFrame::getFrameId( void ) const
{
    return 'T';
}

uint16_t XBeeApiTxFrame::getCmdLen( void ) const
{
    /* Length of the data payload plus the API ID, frame ID and option byte */ 
    uint16_t ret_val = m_dataLen + 3U;
    
    if( m_apiId == XBEE_CMD_TX_16B_ADDR )
    {
        ret_val += 2U;
    }
    else
    {
        ret_val += 8U;
    }

    return ret_val;
}


void XBeeApiTxFrame::getDataPtr( const uint16_t p_start, const uint8_t**  p_buff, uint16_t* const p_len )
{
    if( p_start == 0 ) 
    {
        /* Need to keep the XBEE_API_TX_FRAME_BUFFER_SIZE limit in mind when writing to m_buffer */
 
        uint8_t len = 0;
        m_buffer[ len++ ] = getFrameId(); // Frame ID
        
        /* Pack the destination address depending on whether it's 16 or 64-bit addressed */
        if( m_apiId == XBEE_CMD_TX_16B_ADDR )
        {
            m_buffer[ len++ ] = m_addr >> 8U;
            m_buffer[ len++ ] = m_addr;
        } 
        else
        {
            m_buffer[ len++ ] = m_addr >> 56U;
            m_buffer[ len++ ] = m_addr >> 48U;
            m_buffer[ len++ ] = m_addr >> 32U;
            m_buffer[ len++ ] = m_addr >> 24U;
            m_buffer[ len++ ] = m_addr >> 16U;
            m_buffer[ len++ ] = m_addr >> 8U;
            m_buffer[ len++ ] = m_addr;
        }
        
        /* Frame options */
        m_buffer[ len++ ] =   m_ack?         (0x00U):(0x01U)
                            | m_panBroadcast?(0x04U):(0x00U);
 
        *p_buff = &( m_buffer[0] );
        *p_len = len;    
    } 
    else 
    {
        *p_buff = m_data;
        *p_len = m_dataLen;
    }
}

bool XBeeApiTxFrame::setDataPtr( const uint8_t* const p_buff, const uint16_t p_len )
{
    bool ret_val = false;
    if( p_len < XBEE_API_MAX_TX_PAYLOAD_LEN )
    {
        m_data = p_buff;
        m_dataLen = p_len;
    }
    return ret_val;
}

void XBeeApiTxFrame::setDestAddrType( const XBeeDevice::XBeeApiAddrType_t p_type )
{
    if( p_type == XBeeDevice::XBEE_API_ADDR_TYPE_16BIT )
    {
        m_apiId = XBEE_CMD_TX_16B_ADDR;
    } 
    else 
    {
        m_apiId = XBEE_CMD_TX_64B_ADDR;        
    }
}
       
void XBeeApiTxFrame::setDestAddr( uint64_t p_addr )
{
    m_addr = p_addr;
}

void XBeeApiTxFrame::setDestAddrBroadcast( void )
{
    m_addr = XBEE_BROADCAST_ADDR;
}

void XBeeApiTxFrame::setPanBroadcast( const bool p_bc )
{
    m_panBroadcast = p_bc;
}

bool XBeeApiTxFrame::decodeCallback( const uint8_t* const p_data, size_t p_len )
{
    bool ret_val = false;
 
    if( XBEE_CMD_TX_STATUS == p_data[ XBEE_CMD_POSN_API_ID ] )
    {
        /* Data transmitted call-back */
        frameTxCallback( (XBeeApiTxStatus_e)(p_data[ XBEE_CMD_POSN_ID_SPECIFIC_DATA + 1U ]) );
        ret_val = true;
    }
    
    return ret_val;
}

void XBeeApiTxFrame::frameTxCallback( const XBeeApiTxStatus_e p_status )
{
    /* TODO */
}
