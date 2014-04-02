/**
   @file
   @brief Class to abstract commands send to the XBee API
   
   @author John Bailey 

   @copyright Copyright 2013 John Bailey

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

#if !defined XBEEAPICMD_HPP
#define      XBEEAPICMD_HPP

#include <stdint.h>
#include <stddef.h> // for size_t

/** API identifiers - the first byte in the API-specific structure within an API frame which provides an indication as to the type of data which follows */
typedef enum
{
    XBEE_CMD_TX_64B_ADDR        = 0x00,
    XBEE_CMD_TX_16B_ADDR        = 0x01,
    XBEE_CMD_AT_CMD             = 0x08,
    XBEE_CMD_QUEUE_PARAM_VAL    = 0x09,
    XBEE_CMD_REMOTE_AT_CMD      = 0x17,
    XBEE_CMD_RX_64B_ADDR        = 0x80,
    XBEE_CMD_RX_16B_ADDR        = 0x81,
    XBEE_CMD_AT_RESPONSE        = 0x88,
    XBEE_CMD_TX_STATUS          = 0x89,
    XBEE_CMD_MODEM_STATUS       = 0x8A,
    XBEE_CMD_REMOTE_AT_RESPONSE = 0x97,
    XBEE_CMD_INVALID            = 0xFFF   
} XBeeApiIdentifier_e; 

/** Position of fixed meaning bytes within the API frame, relative to the start of the frame */
enum
{
    /** Start delimiter, should be equal to XBEE_SB_FRAME_DELIMITER */
    XBEE_CMD_POSN_SDELIM = 0x00,
    /** High byte of the 16-bit length field */
    XBEE_CMD_POSN_LEN_HI = 0x01,
    /** Low byte of the 16-bit length field */
    XBEE_CMD_POSN_LEN_LO = 0x02,
    /** API identifier for the data which follows - see XBeeApiIdentifier_e */
    XBEE_CMD_POSN_API_ID = 0x03,
    /** Start of API identifier specific data */
    XBEE_CMD_POSN_ID_SPECIFIC_DATA = 0x04
};

/** Helper macro to retrieve the frame payload length (i.e. excluding overhead - see XBEE_API_FRAME_OVERHEAD) from a buffer.

    \param _b Pointer to a buffer containing a received API frame.
    \returns Length of the frame payload excluding overhead */
#define MSG_LEN_IN_BUFFER( _b ) ((((uint16_t)_b[ XBEE_CMD_POSN_LEN_HI ]) << 8U) + _b[XBEE_CMD_POSN_LEN_LO])

/** The number of 'overhead' bytes in an API frame (i.e. those not included in the frame payload, namely the start delimiter, 2 length bytes & checksum */
#define XBEE_API_FRAME_OVERHEAD 4U

/* Forward declare this as XBeeDevice is dependent upon XBeeApiFrameDecoder */
class XBeeDevice;

/** Class which represents an API frame, exchanged with the XBee.
    This class in itself will not create a valid API frame and needs to be sub-classed.
*/
class XBeeApiFrame
{
    private:
        static const   uint8_t m_cmdHeaderLen = 1;

    protected:
        /** API identifier for the data in the frame */
        XBeeApiIdentifier_e m_apiId;
        
        /** API identifier specific data */
        const uint8_t*      m_data;
        
        /** Length of the data pointed to by m_data */
        uint16_t            m_dataLen;

    public:
    
        /** Constructor */
        XBeeApiFrame( void );
    
        /** Constructor */
        XBeeApiFrame( XBeeApiIdentifier_e p_id,
                      const uint8_t* const p_data,
                      const size_t         p_dataLen );

        /** Return the length of the API-specific structure represented by this frame.  i.e. the API frame without the overhead - see XBEE_API_FRAME_OVERHEAD */
        virtual uint16_t getCmdLen( void ) const;
    
        /** Retrieve the API identifier for this frame */
        XBeeApiIdentifier_e getApiId( void ) const;
        
        /** Retrieve a pointer to the API-specific data (i.e. that which follows the API identifier in 
            the frame).  In *total*, getDataPtr will return sufficient data to match getCmdLen(), however
            it may be done in chunks using p_start and *p_len to control the fragmentation process.  
            For example:
            
                getCmdLen() == 5;
                getDataPtr( 0, &b, &l );
                l == 2; b[0] == 'a'; b[1] == 'b'; 
                getDataPtr( 2, &b, &l );
                l == 1; b[2] == 'c';
                getDataPtr( 3, &b, &l );
                l == 2; b[0] == 'd'; b[1] == 'e';
            
            \param[in]  p_start The start point within the buffer.  It is only meaningful to use a non-zero
                                value in the case that a previous call to getDataPtr did not return a buffer
                                of length getCmdLen(), in which case p_start should be set to be the next
                                byte that is required.
            \param[out] p_buff  Pointer to a pointer to receive the buffer pointer 
            \param[out] p_len   Pointer to receive the length of the data pointed to by *p_buff */
        virtual void getDataPtr( const uint16_t p_start, const uint8_t**  p_buff, uint16_t* const p_len ) const;
};

/** Class which acts as a receiver for data from the XBee and takes care of decoding it.
    This class needs to be sub-classed in order to create a valid decoder.
    
    An object implementing the XBeeApiFrameDecoder interface should be registered with
    an XBeeDevice-type object using the registerDecoder() method.  If the class destructor
    is called it will automatically un-register itself from the XBeeDevice.  Alternatively
    unregisterDecoder() may be called */
class XBeeApiFrameDecoder
{
    protected:
        /** Pointer to the XBeeDevice with which this decoder is associated.  Kept around
            so that we know where to go at time of destruction and the XBeeDevice isn't
            left with a pointer to an invalidated object */
        XBeeDevice* m_device;
        
    public:
        
        /** Constructor */
        XBeeApiFrameDecoder( XBeeDevice* const p_device = NULL );
        
        /** Destructor.  Un-registers the decoder from any XBeeDevice object with which it is registered */
        virtual ~XBeeApiFrameDecoder();
    
    /** XBeeDevice is a friend so that it can access registerCallback and unregisterCallback */
    friend class XBeeDevice;
    
    protected:      
    
        /** Called by an XBeeDevice object to let this object know that it's being associated with a device */      
        void registerCallback( XBeeDevice* const p_device );

        /** Called by an XBeeDevice object to let this object know that it's no longer associated with the device */ 
        void unregisterCallback( void );

        /** Called by an XBeeDevice in order to give this object the opportunity to examine and decode data received
            from the XBee
            
            Note: When implementing this method in an inheriting class the method must only return true in the case that
                  the data was intended for and was decoded by this class.  Returning true in other cases may result in
                  other decoders being denied the opportunity to examine the data
            
            \param p_data Pointer to the first byte of the data buffer (i.e. XBEE_CMD_POSN_SDELIM).  The implementation
                          of any over-riding function should not expect that the data referenced by this pointer will
                          remain valid after the call-back has completed.
            \param p_len  Size of the data pointed to by p_data
            \returns true in the case that the data was examined and decoded successfully
                     false in the case that the data was not of interest or was not decoded successfully
        */
        virtual bool decodeCallback( const uint8_t* const p_data, size_t p_len ) = 0;    
};

/** Value which represents the broadcast address */
#define XBEE_BROADCAST_ADDR 0xFFFF


#endif
