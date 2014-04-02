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

#include "XBeeApiRxFrameCircularBuffer.hpp"

XBeeApiRxFrameCircularBuffer::XBeeApiRxFrameCircularBuffer( size_t p_bufferSize, XBeeDevice* p_device ) : XBeeApiRxFrameDecoder( p_device ),
										     m_bufferSize( p_bufferSize ),
										     m_head( 0 ),
										     m_tail( 0 ),
										     m_count( 0 )
{
    m_framesBuffer = new( XBeeApiRxFrame[ m_bufferSize ] );
}

XBeeApiRxFrameCircularBuffer::~XBeeApiRxFrameCircularBuffer( void )
{
}

void XBeeApiRxFrameCircularBuffer::frameRxCallback( const XBeeApiRxFrame* const p_frame )
{
    if( m_count < m_bufferSize )
    {
	size_t insertPoint = m_head;
	m_count++;
        m_head = (m_head + 1) % m_bufferSize;
	m_framesBuffer[ insertPoint ].deepCopyFrom( *p_frame );
    }
}
	
size_t XBeeApiRxFrameCircularBuffer::getFrameCount() const
{
    return( m_count );
}

void XBeeApiRxFrameCircularBuffer::clear()
{
    m_count = 0;
}

void XBeeApiRxFrameCircularBuffer::pop()
{
    if( m_count )
    {
    	m_tail = (m_tail + 1) % m_bufferSize;
    	m_count--;
    }
}
	
const XBeeApiRxFrame* XBeeApiRxFrameCircularBuffer::getTailPtr( void ) const
{
    XBeeApiRxFrame* ret_val = NULL;
    if( m_count )
    {
        ret_val = &( m_framesBuffer[ m_tail ] );
    }
    return ret_val;
}
