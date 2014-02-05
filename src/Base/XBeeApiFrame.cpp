/** 

Copyright 2013 John Bailey
   
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

#include "XBeeApiFrame.hpp"
#include "XBeeDevice.hpp"

#include <stdlib.h>

XBeeApiFrame::XBeeApiFrame( void ) : m_apiId ( XBEE_CMD_INVALID ), m_data( NULL ), m_dataLen( 0 )
{
}

XBeeApiFrame::XBeeApiFrame( XBeeApiIdentifier_e p_id,
                            const uint8_t* p_data,
                            const size_t   p_dataLen ) : m_apiId( p_id ), m_data( p_data ), m_dataLen( p_dataLen )
{
}

uint16_t XBeeApiFrame::getCmdLen( void ) const
{
    return m_dataLen + m_cmdHeaderLen;    
}


XBeeApiIdentifier_e XBeeApiFrame::getApiId( void ) const
{
    return m_apiId;
}
        
void XBeeApiFrame::getDataPtr( const uint16_t p_start, const uint8_t**  p_buff, uint16_t* const p_len )
{
    *p_len = m_dataLen;
    (*p_buff) = m_data;
}

XBeeApiFrameDecoder::XBeeApiFrameDecoder( XBeeDevice* const p_device ) : m_device( NULL )
{
    if( p_device != NULL )
    {
        p_device->registerDecoder( this );
    } 
}

XBeeApiFrameDecoder::~XBeeApiFrameDecoder()
{
    if( m_device != NULL )
    {
        m_device->unregisterDecoder( this );
    }    
}

void XBeeApiFrameDecoder::registerCallback( XBeeDevice* const p_device )
{
    m_device = p_device;
}

void XBeeApiFrameDecoder::unregisterCallback( void )
{
    m_device = NULL;
}