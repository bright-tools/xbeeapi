/**
   @file
   @brief Class to support reception of data via the XBee's wireless
      
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

#if !defined XBEEAPIRXFRAME_HPP
#define      XBEEAPIRXFRAME_HPP

#include "XBeeApiFrame.hpp"
#include "XBeeDevice.hpp"

#include <stdint.h>

/** Class to represent a frame of data being received by the XBee.

    The message data content is accessed via the inherited m_data 
    and m_dataLen members 
*/
class XBeeApiRxFrame : public XBeeApiFrame
{
    protected:
        /** The source address of the packet */
        uint64_t m_addr;
    
        /** Indicate whether or not the address in m_addr is 16- or 64-bit */ 
        bool m_addrIs16bit;
    
        /** The Received Signal Strength Indicator (RSSI) associated with the
            message, in -dBm.  i.e. m_rssi == 40 indicates -40dBm */
        uint8_t m_rssi;
    
        /** Indicate whether or not the message was address broadcast */
        bool m_addressBroadcast;
        
        /** Indicate whether or not the message was PAN broadcase */
        bool m_panBroadcast;
   
	/* TODO: doc */
       	bool m_dataIsMallocd;	

	/* TODO: doc */
       	uint8_t* m_mallocdData;	
    public:
        /** Constructor */
        XBeeApiRxFrame();
        
	/** Constructor */
        XBeeApiRxFrame( XBeeApiIdentifier_e p_id,
                        const uint8_t* const p_data,
                        const size_t         p_dataLen );

	/** Deep copy */
	/* TODO: doc */
        bool deepCopyFrom( const XBeeApiRxFrame& p_frame );
       
        /** Destructor */
        virtual ~XBeeApiRxFrame( void ); 
};

#endif
