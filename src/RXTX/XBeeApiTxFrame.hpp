/**
   @file
   @brief Class to support transmission of data via the XBee's wireless
      
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

#if !defined XBEEAPITXFRAME_HPP
#define      XBEEAPITXFRAME_HPP

#include "XBeeApiFrame.hpp"
#include "XBeeDevice.hpp"

#include <stdint.h>

#define XBEE_API_TX_FRAME_BUFFER_SIZE 11U

#define XBEE_API_MAX_TX_PAYLOAD_LEN 100U

/** Class to represent a frame of data being transmitted by the XBee */
class XBeeApiTxFrame : public XBeeApiFrame, public XBeeApiFrameDecoder
{
    protected:
        /** Destination address of the frame */
        uint64_t          m_addr;
        
       /** Whether or not an acknowledgement of the frame is requested */ 
       bool              m_ack;
       /** Whether or not the frame is a PAN broadcast */
       bool              m_panBroadcast;
       /** Buffer to house data relating to the frame header */
       uint8_t           m_buffer[XBEE_API_TX_FRAME_BUFFER_SIZE];

       /** Called by XBeeDevice in order to offer frame data to the object for
           decoding
           
           \param p_data Pointer to the content of the received data
           \param p_len Length of the data pointed to by p_data
       */
       virtual bool decodeCallback( const uint8_t* const p_data, size_t p_len );

    public:
       /** Enum for capturing the possible status of an XBee message TX 
           attempt */
       typedef enum
       {
           /** Frame was transmitted OK.  Note that successfully transmitted 
               broadcast frames will have this status, even in the case
               that they are not received by any other nodes */
           XBEE_API_TX_STATUS_OK = 0,
           /** No acknowledgement was received when the frame was transmitted.
               See XBEE_API_TX_STATUS_OK for note regarding broadcast frames  */
           XBEE_API_TX_STATUS_NO_ACK = 1,
           /** Channel is unavailable for transmission (Clear Channel 
               Assessment failed) */
           XBEE_API_TX_STATUS_CCA_FAIL = 2,
           /** Coordinator has timed out an indirect transmission */
           XBEE_API_TX_STATUS_PURGED = 3,
           /** Not an actual status, used to provide a handle on the numer of 
               enumeration items */
           XBEE_API_TX_STATUS_LAST = 4  
       } XBeeApiTxStatus_e;
    
       XBeeApiTxFrame( XBeeDevice* p_device = NULL );
       virtual ~XBeeApiTxFrame( void );
       
       void setDestAddrType( const XBeeDevice::XBeeApiAddrType_t p_type );
       void setDestAddr( uint64_t p_addr );
       void setDestAddrBroadcast( void );
       void setPanBroadcast( const bool p_bc );
       
       virtual uint16_t getCmdLen( void ) const;
       virtual void getDataPtr( const uint16_t p_start, const uint8_t**  p_buff, uint16_t* const p_len );
       
       virtual uint8_t getFrameId( void ) const;
       
       /** Callback function which is invoked when a response to the TX request is received from
           the XBee.
           
           \param p_status Status of the TX attempt */
       virtual void frameTxCallback( const XBeeApiTxStatus_e p_status );
       
       /** Set the frame payload
       
           \param p_buff Pointer to the buffer containing the data.  Note that this buffer is not copied, so
                         must retain the appropriate content until transmission is complete
           \param p_len Length of the data pointed to be p_buff.  Must be 100 or less.
           \returns true in the case that the operation was successful, false in the case that it was not
                    (content too long, etc)
       */
       bool setDataPtr( const uint8_t* const p_buff, const uint16_t p_len );
};

#endif