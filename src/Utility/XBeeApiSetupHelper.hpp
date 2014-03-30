/**
   @file
   @brief Functions to help with XBee configuration
      
   @author John Bailey 

   @copyright Copyright 2014 John Bailey

   @section LICENSE
   
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

#if !defined XBEEAPISETUPHELPER_HPP
#define      XBEEAPISETUPHELPER_HPP

#include "XBeeDevice.hpp"
#include "XBeeApiCmdAt.hpp"

typedef enum {
    XBEE_NETWORK_TYPE_P2P,
    XBEE_NETWORK_TYPE_NON_BEACON,
    XBEE_NETWORK_TYPE_UNKNOWN
} XBeeNetworkType_e;

/** Set the XBee to use the P2P networking model.  Thie helper function
    assumes that the XBee device and the passed XBeeApiCmdAt-type object have already
    been configured.

    This function will block until the settings have been successfully applied or a failure
    has been encountered.

    \param p_xbeeCmd Pointer to an XBeeApiCmdAt object which has already been registered
                     as a decoder with an XBee device.
    \param p_id Network ID to use
    \param p_chan Channel to use
    \return true in the case that the XBee was configured, false in the case that a problem
            was encountered
*/      
extern bool xbeeSetNetworkTypeP2P( XBeeApiCmdAt* const           p_xbeeCmd,
                                   const XBeeApiCmdAt::panId_t   p_id,
                                   const XBeeApiCmdAt::channel_t p_chan );

#endif /* !defined( XBEEAPISETUPHELPER_HPP ) */
