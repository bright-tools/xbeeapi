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

#include "XBeeApiRxFrame.hpp"

XBeeApiRxFrame::XBeeApiRxFrame( void ) : XBeeApiFrame(),
					 m_dataIsMallocd ( false )
{
}

	/** Constructor */
XBeeApiRxFrame::XBeeApiRxFrame( XBeeApiIdentifier_e p_id,
                                const uint8_t* const p_data,
                                const size_t         p_dataLen ) : XBeeApiFrame( p_id, p_data, p_dataLen ),
							           m_dataIsMallocd ( false )
{
}

XBeeApiRxFrame::~XBeeApiRxFrame( void )
{
    if( m_dataIsMallocd )
    {
        free( m_mallocdData );
    }
}
        
bool XBeeApiRxFrame::deepCopyFrom( const XBeeApiRxFrame& p_frame )
{
    /* TODO: need to catch assignment operator and free data if data is already malloc'd */
    if( m_dataIsMallocd )
    {
        free( m_mallocdData );
	m_dataIsMallocd = false;
    }
    *this = p_frame;
    m_mallocdData = (uint8_t*)malloc( p_frame.m_dataLen );
    if( m_mallocdData )
    {
    	m_dataIsMallocd = true;
        memcpy( m_mallocdData, p_frame.m_data, m_dataLen );
	m_data = m_mallocdData;
    }
    else
    {
        m_data = NULL;
    }
    return( m_mallocdData != NULL );
}
