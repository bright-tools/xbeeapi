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

#include "XBeeApiRxFrameDecoder.hpp"

XBeeApiRxFrameDecoder::XBeeApiRxFrameDecoder( XBeeDevice* p_device ) : XBeeApiFrameDecoder( p_device )
{
}

XBeeApiRxFrameDecoder::~XBeeApiRxFrameDecoder( void )
{
}

bool XBeeApiRxFrameDecoder::decodeCallback( const uint8_t* const p_data, size_t p_len )
{
    bool ret_val = false;
 
    /* TODO: Length check */
 
    if(( XBEE_CMD_RX_64B_ADDR == p_data[ XBEE_CMD_POSN_API_ID ] ) ||
       ( XBEE_CMD_RX_16B_ADDR == p_data[ XBEE_CMD_POSN_API_ID ] ))
    {
        size_t pos = XBEE_CMD_POSN_ID_SPECIFIC_DATA;
	uint64_t addr;
	bool addrIs16bit;
	uint8_t rssi;
	bool addressBroadcast;
        bool panBroadcast;
	const uint8_t* data;
        size_t   dataLen;

        /* Depending on the frame type, decode either a 64- or 16-bit address */
        if( XBEE_CMD_RX_64B_ADDR == p_data[ XBEE_CMD_POSN_API_ID ] ) 
        {
            addr = (((uint64_t)p_data[ pos ]) << 54U ) | 
                   (((uint64_t)p_data[ pos+1 ]) << 48U ) |
                   (((uint64_t)p_data[ pos+2 ]) << 40U ) |
                   (((uint64_t)p_data[ pos+3 ]) << 32U ) |
                   (((uint64_t)p_data[ pos+4 ]) << 24U ) |
                   (((uint64_t)p_data[ pos+5 ]) << 16U ) |
                   (((uint64_t)p_data[ pos+6 ]) << 8U ) |
                               p_data[ pos+7 ];
            pos += sizeof( uint64_t );
            addrIs16bit = false;
        }
        else
        {
            addr = (((uint16_t)p_data[ pos ]) << 8U ) | 
                                 p_data[ pos+1 ];
            pos += sizeof( uint16_t );
            addrIs16bit = true;
        }
        
        rssi += p_data[ pos++ ];
        
        addressBroadcast = p_data[ pos ] & 2U;
        panBroadcast = p_data[ pos ] & 4U;
        pos++;
        
        data = &( p_data[ pos ] );
        
        /* -1 to account for the checksum */
        dataLen = p_len - pos - 1;
        
	XBeeApiRxFrame new_frame( (XBeeApiIdentifier_e)(p_data[ XBEE_CMD_POSN_API_ID ]),
			          data,
				  dataLen );
        
        frameRxCallback( &new_frame );
        
        ret_val = true;
    }
    
    return ret_val;
}
