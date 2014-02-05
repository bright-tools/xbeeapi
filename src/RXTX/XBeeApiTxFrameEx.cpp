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

#include "XBeeApiTxFrameEx.hpp"

XBeeApiTxFrameEx::XBeeApiTxFrameEx( XBeeDevice* p_device ) : XBeeApiTxFrame( p_device ),
                                                             m_recent( XBeeApiTxFrame::XBEE_API_TX_STATUS_LAST )
{
    uint16_t i;
    
    for( i = 0; i < XBEE_API_TX_STATUS_LAST; ++i )
    {
        m_statusCounters[ i ] = 0;
    }
}


XBeeApiTxFrameEx::~XBeeApiTxFrameEx()
{
}

XBeeApiTxFrame::XBeeApiTxStatus_e XBeeApiTxFrameEx::getMostRecentStatus( void ) const
{
    return m_recent;
}

void XBeeApiTxFrameEx::frameTxCallback( const XBeeApiTxStatus_e p_status )
{
    XBeeApiTxFrame::frameTxCallback( p_status );
    
    if( p_status < XBEE_API_TX_STATUS_LAST )
    {
        m_recent = p_status;
        m_statusCounters[ p_status ]++;
    }
}

uint16_t XBeeApiTxFrameEx::getStatusCount( const XBeeApiTxStatus_e p_status )
{
    uint16_t ret_val = 0;

    if( p_status < XBEE_API_TX_STATUS_LAST )
    {
        ret_val = m_statusCounters[ p_status ];
    }

    return ret_val;
}