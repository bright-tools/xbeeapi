/**
   @file
   @brief Class to abstract the XBee serial interface
      
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

#if !defined XBEEDEVICE_HPP
#define      XBEEDEVICE_HPP

#include "XBeeApiCfg.hpp"

#include "mbed.h" // For serial interface
#if defined  XBEEAPI_CONFIG_USING_RTOS
#include "rtos.h" // Mutex support
#endif

#include "FixedLengthList.hpp"
#include "CircularBuffer.h"

#include "XBeeApiFrame.hpp"

/** Class to represent an XBee device & provide an interface to communicate with it

    Actual communication is performed by:
    tx - using SendFrame to transmit messages to the XBee
    rx - registering one or more decoders (via registerDecoder) to be called when
         a message is received */
class XBeeDevice
{
   private:
 
#if defined  XBEEAPI_CONFIG_USING_RTOS
     /** Mutex for accessing the serial interface */
     rtos::Mutex  m_ifMutex;
#endif
   
     /** Track whether the XBee is in CMD mode or API mode */
     bool m_inAtCmdMode;
   
     /** Track whether or not the last byte received from the XBee was an escape (i.e. the 
     next incoming byte needs to be un-escaped) */
     uint16_t m_rxMsgLastWasEsc;
   
     /** Serial interface for the XBee comms */
     Serial m_if;
     
     /** Call-back function from MBED triggered when data is
         received on the XBee's serial interface */
     void if_rx( void );
     
     /** Helper function to determine whether or not there's a message to decode and to
         offer it round any registered decoders */
     void checkRxDecode( void );

     /** Write a byte to the XBee serial interface, taking care of any
         escaping requirements (see m_escape)
         
         @param p_byte Byte to be written 
         @return Sum of the byte(s) written - to be addded to the checksum 
     */
     uint8_t xbeeWrite( uint8_t p_byte, bool p_doEscape = true );

     /** Flag to indicate whether or not the dataflow is currentl being escaped */
     bool m_escape;
     
     /** Buffer of bytes received from the XBee so far */
     CircularBuffer<XBEEAPI_CONFIG_RX_BUFFER_SIZE> m_rxBuff;
     
     /** List of objects which are registered to de-code received frames */
     FixedLengthList<XBeeApiFrameDecoder*, XBEEAPI_CONFIG_DECODER_LIST_SIZE> m_decoders;
     
   public:
   
     /** Represent the status of an XBee message exchange */
     typedef enum {
         /** Successful communication */
         XBEEDEVICE_OK                = 0,
         /** No response received from the XBee within the expected time */
         XBEEDEVICE_TIMEOUT           = 1,
         /** Data received from the XBee, but was of the wrong length */
         XBEEDEVICE_UNEXPECTED_LENGTH = 2,
         /** Data received from the XBee was in an unexpected format */
         XBEEDEVICE_UNEXPECTED_DATA   = 3,
         /** XBee is currently in the wrong mode to support this request (e.g. requesting AT ASCII
             communications when in API mode) */
         XBEEDEVICE_WRONG_MODE        = 4,
     } XBeeDeviceReturn_t;
   
     /** Represents the different types of addressing for XBee devices */
     typedef enum {
         XBEE_API_ADDR_TYPE_16BIT = 0,
         XBEE_API_ADDR_TYPE_64BIT = 1    
     } XBeeApiAddrType_t;

     /** Constructor.  Parameters are used to specify the particulars of the connection to the XBee
     
         @param p_tx Serial interface TX pin
         @param p_rx Serial interface RX pin
         @param p_rts Pin to use for RTS (flow control).  Will only be used if supported.  Can specify NC to disable.
         @param p_rts Pin to use for CTS (flow control).  Will only be used if supported.  Can specify NC to disable.
     */
     XBeeDevice( PinName p_tx, PinName p_rx, PinName p_rts, PinName p_cts );  

     virtual ~XBeeDevice( void );  
     
     /** Transmit the specified frame to the XBee.  This method does not block waiting for a response, but returns and
         expects that any response will be dealt with by an appropriately registered decoder
     
         The XBee represents frames as:
    
         --------------------------------------------------------------------------
        | Start Delimiter | Length  | API identifier | ID specific data | Checksum |
        |  1 byte         | 2 bytes | 1 byte         | x bytes          | 1 byte   |
        |      0x7e       | MSB LSB | APIID          | ... ... ...      | CKSUM    |
         --------------------------------------------------------------------------
    
         The method uses the XBeeApiFrame class methods to fill in the length, API ID & data.
     
         \param p_cmd Frame to be transmitted
     */
     void SendFrame( XBeeApiFrame* const p_cmd );
     
     /** Set the XBee up in API mode.  Note that this method needs to know something about the way in which the
         attached XBee is configured (namely the guard time).  This is configured via XBeeApiCmd.hpp, currently */
     XBeeDeviceReturn_t setUpApi( void );
          
     /** Register an object as being interested in decoding messages from the XBee.  Note that each
         decoder MUST only be registered with ONE XBeeDevice.

         \param p_decoder Decoder to be registered
         \returns true in the case that registration was successful, false otherwise (decoder list full, decoder already registered, etc) */
     bool registerDecoder( XBeeApiFrameDecoder* const p_decoder );
     
     /** Remove a previous registration for decoding of messages.  The decoder will be removed from
         the list and no-longer called when XBee data is received
         
         \param p_decoder Decoder to be unregistered
         \returns true in the case that unregistration was successful, false otherwise (decoder not in list) */
     bool unregisterDecoder( XBeeApiFrameDecoder* const p_decoder );
     
#if defined XBEEAPI_CONFIG_ENABLE_DEVELOPER
     void dumpRxBuffer( Stream* p_buf, const bool p_hexView );
#endif

  protected:
     /** Send an ASCII frame to the XBee.  This method blocks until a response is received
         or a timeout occurs.
         
         Note that this is a protected method as it is here to support setUpApi() with regard
         to getting the XBee into API mode.  It's not intended to be used for general
         communication with the XBee.
     
         \param p_dat ASCII data to be sent
         \param p_len Length of the data pointed to by p_dat
         \param p_wait_ms Time to wait for a response
     */
     XBeeDeviceReturn_t SendFrame( const char* const p_dat, size_t p_len, int p_wait_ms = 1000 );
     


};

#endif