/**
   @file
   @brief Configuration options for the XBeeApi module
   
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

#if !defined XBEEAPICFG_HPP
#define      XBEEAPICFG_HPP

/** Set the size of the RX buffer in the XBeeDevice */
#define XBEEAPI_CONFIG_RX_BUFFER_SIZE 512

/** Enable developer options */
#define XBEEAPI_CONFIG_ENABLE_DEVELOPER

/** Maximum number of decoders to be supported.  Attempting to add more
    than the maximum will fail.  Increating this will use slightly more 
    memory */
#define XBEEAPI_CONFIG_DECODER_LIST_SIZE 10

/** Guard period for sending "+++" commands - see XBee documentation */
#define XBEEAPI_CONFIG_GUARDPERIOD_MS 1000

#if 0
/** Use RTOS features to make XBeeApi threadsafe */
#define  XBEEAPI_CONFIG_USING_RTOS
#endif

#if 0
#define XBEE_DEBUG_DEVICE_DUMP_MESSAGE_DECODE
#endif

#endif /* !defined( XBEEAPICFG_HPP ) */