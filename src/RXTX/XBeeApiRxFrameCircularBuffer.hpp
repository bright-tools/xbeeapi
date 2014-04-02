/**
   @file
   @brief Class to decode and store received data frames in a circular
           buffer
      
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

#if !defined XBEEAPIRXFRAMECIRCULARBUFFER_HPP
#define      XBEEAPIRXFRAMECIRCULARBUFFER_HPP

#include "XBeeApiRxFrameDecoder.hpp"
#include "XBeeDevice.hpp"

#include <stdint.h>

/** Class to deal with decoding of an RX'd data frame
*/
class XBeeApiRxFrameCircularBuffer : public XBeeApiRxFrameDecoder
{
    protected:
        size_t          m_bufferSize;
        size_t          m_head;
        size_t          m_tail;	
	size_t          m_count;
	XBeeApiRxFrame* m_framesBuffer;
    public:
        /** Constructor */
        XBeeApiRxFrameCircularBuffer( size_t p_bufferSize, XBeeDevice* p_device = NULL );
        
        /** Destructor */
        virtual ~XBeeApiRxFrameCircularBuffer( void ); 

        /* Callback which is invoked when a frame is successfully decoded
	   \param p_frame The frame content
        */       
        virtual void frameRxCallback( const XBeeApiRxFrame* const p_frame );

	size_t getFrameCount() const;

	void clear();

	void pop();

	const XBeeApiRxFrame* getTailPtr() const;
};

#endif
