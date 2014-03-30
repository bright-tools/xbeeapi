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

#include "XBeeApiSetupHelper.hpp"

bool xbeeSetNetworkTypeP2P( XBeeApiCmdAt* const p_xbeeCmd,
                            const XBeeApiCmdAt::panId_t p_id,
                            const XBeeApiCmdAt::channel_t p_chan )
{    
    bool ret_val = p_xbeeCmd->setCoordinatorEnabled( false );
    if( ret_val ) 
    {
    	ret_val = p_xbeeCmd->setEndDeviceAssociationEnabled( false );
    }
    if( ret_val ) 
    {
    	p_xbeeCmd->setChannel( p_chan );
    }
    if( ret_val ) 
    {
    	p_xbeeCmd->setPanId( p_id );
    }
    return ret_val;
}

