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

#if !defined XBEEAPIRXFRAMEDECODER_HPP
#define      XBEEAPIRXFRAMEDECODER_HPP

#include "XBeeApiRxFrame.hpp"
#include "XBeeDevice.hpp"

#include <stdint.h>

/** Class to deal with decoding of an RX'd data frame
*/
class XBeeApiRxFrameDecoder : public XBeeApiFrameDecoder
{
    protected:
        /** Called by XBeeDevice in order to offer frame data to the object for
            decoding
           
            \param p_data Pointer to the content of the received data
            \param p_len Length of the data pointed to by p_data
        */
        virtual bool decodeCallback( const uint8_t* const p_data, size_t p_len );

    public:
        /** Constructor */
        XBeeApiRxFrameDecoder( XBeeDevice* p_device = NULL );
        
        /** Destructor */
        virtual ~XBeeApiRxFrameDecoder( void ); 

        /* Callback which is invoked when a frame is successfully decoded
	   \param p_frame The frame content
        */       
        virtual void frameRxCallback( const XBeeApiRxFrame* const p_frame ) = 0;
};

#endif
