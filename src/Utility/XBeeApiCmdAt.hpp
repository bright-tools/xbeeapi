/**
   @file
   @brief Class to abstract AT commands send to the XBee API
   
   AT commands have the payload:
   
     Byte 1     : Frame ID
     Byte 2 & 3 : AT command
     Byte 4-n   : Parameter Value 
   
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

#if !defined XBEEAPICMDAT_HPP
#define      XBEEAPICMDAT_HPP

#include "XBeeApiFrame.hpp"
#include "XBeeDevice.hpp"

#include <stdint.h>

#define XBEE_API_CMD_SET_HEADER_LEN 3U

/** Class to access the configuration interface of the XBee.
    Requests to the XBee are non-blocking meaning that code
    which utilises this class must deal with the fact that
    there will be a delay between requesting the data from
    the XBee and the data being available via the API.  See
    XBeeApiCmdAtBlocking for a blocking version.
    
    Parameters from the XBee are cached in the object so
    subsequent requests do not need have the overhead of
    communication with the XBee */
class XBeeApiCmdAt : public XBeeApiFrameDecoder
{
    public:
        /** Type to represent the ID of a PAN (Personal Area Network) */
        typedef uint16_t panId_t;
        /** Type to represent a wireless channel number */
        typedef uint8_t  channel_t;
        /** Type to represent the different MAC modes supported by the XBee */
        typedef enum {
            XBEE_API_MAC_MODE_DIGI_ACK        = 0,
            XBEE_API_MAC_MODE_802_15_4_NO_ACK = 1,
            XBEE_API_MAC_MODE_802_15_4_ACK    = 2,
            XBEE_API_MAC_MODE_DIGI_NO_ACK     = 3,
        } XBeeApiMACMode_e;
    
    protected:
        /** Indicates whether or not m_hwVer contains data retrieved from the XBee */
        bool m_have_hwVer;
        /** Indicates whether or not m_fwVer contains data retrieved from the XBee */
        bool m_have_fwVer;
        /** Indicates whether or not m_chan contains data retrieved from the XBee */
        bool m_have_chan;
        /** Indicates whether or not m_PANId contains data retrieved from the XBee */
        bool m_have_PANId;
        /** Indicates whether or not m_EDA contains data retrieved from the XBee */
        bool m_have_EDA;
        /** Indicates whether or not m_CE contains data retrieved from the XBee */
        bool m_have_CE;
        bool m_have_sourceAddress;
        bool m_have_snLow;
        bool m_have_snHigh;
        bool m_have_retries;
        bool m_have_randomDelaySlots;
        bool m_have_macMode;
        
        uint16_t m_hwVer;
        uint16_t m_fwVer;
        channel_t m_chan;
        channel_t m_chanPend;
        panId_t m_PANId;
        panId_t m_PANIdPend;
        bool m_EDA;
        bool m_EDAPend;
        bool m_CE;
        bool m_CEPend;
        uint16_t m_sourceAddress;
        uint16_t m_sourceAddressPend;
        uint32_t m_snLow;
        uint32_t m_snHigh;
        uint8_t  m_retries;
        uint8_t  m_retriesPend;
        uint8_t  m_randomDelaySlots;
        uint8_t  m_randomDelaySlotsPend;
        XBeeApiMACMode_e m_macMode;
        XBeeApiMACMode_e m_macModePend;

        /** Template class to create an XBeeApiFrame which can be used to change
            the value of one of the XBee parameters.  This class is used by the
            setXXX methods in XBeeApiCmdAt */
        template< typename T >
        class XBeeApiCmdAtSet : public XBeeApiFrame {
            uint8_t m_buffer[ XBEE_API_CMD_SET_HEADER_LEN + sizeof( T ) ];
            public:
                /** Constructor
                   
                    \param p_data Pointer to a buffer of length 3 bytes containing a 
                                  single byte frame ID followed by 2 bytes identifying
                                  the command, e.g. '0', 'V', 'R' would set up a version
                                  request with a frame identifier of 48 (ASCII value of 
                                  '0').
                    \param p_val New value for the parameter 
                */
                XBeeApiCmdAtSet( const uint8_t* const p_data,
                                 const T p_val );
                /** Destructor */
                virtual ~XBeeApiCmdAtSet();
        };

       /* Implement XBeeApiCmdDecoder interface */
       virtual bool decodeCallback( const uint8_t* const p_data, size_t p_len );

    public:

        /** Constructor 
        
            \param p_device XBee device with which this object should be associated */
        XBeeApiCmdAt( XBeeDevice* const p_device = NULL );
       
        /** Destructor */
        virtual ~XBeeApiCmdAt( void ) {};
       
        /** Request the hardware version identifier from the XBee.
            As the data is retrieved asynchronously to this call,
            once the response is received it can be accessed via
            getHardwareVersion() */
        bool requestHardwareVersion( void );
        bool requestFirmwareVersion( void );
        bool requestChannel( void );
        bool requestCoordinatorEnabled( void );
        bool requestEndDeviceAssociationEnabled( void );
        bool requestPanId( void );
        bool requestSourceAddress( void );
        bool requestSerialNumber( void );
        /** Request the number of retries the XBee is configured to make.
            Note that the 802.15.4 MAC already allows 3 retries, so this parameter
            specifies an additional multiple of 3 retries
            As the data is retrieved asynchronously to this call,
            once the response is received it can be accessed via
            getRetries() 
        */
        bool requestRetries( void );
        
        /** Request the minimum value of the back-off exponent in the CSMA-CA algorithm
            used in collision avoidance.
        */
        bool requestRandomDelaySlots( void );
        
        bool requestMacMode( void );

        /** Read the XBee's hardware version identifier.  
       
            This method does not initiate any communication with the
            XBee - the identifier must previously have been requested 
            via requestHardwareVersion().  The method is non-blocking. */
        virtual bool getHardwareVersion( uint16_t* const p_ver );

        /** Read the XBee's firmware version identifier.  
       
            This method does not initiate any communication with the
            XBee - the identifier must previously have been requested 
            via requestFirmwareVersion().  The method is non-blocking. */
        virtual bool getFirmwareVersion( uint16_t* const p_ver );

        virtual bool getSerialNumber( uint64_t* const p_sn );
       
        virtual bool getChannel( uint8_t* const p_chan );
        virtual bool setChannel( uint8_t const p_chan );
       
        virtual bool getCoordinatorEnabled( bool* const p_en );
        virtual bool setCoordinatorEnabled( const bool p_en );
       
        virtual bool getEndDeviceAssociationEnabled( bool* const p_en ); 
        virtual bool setEndDeviceAssociationEnabled( const bool p_en  );
       
        virtual bool getPanId( panId_t* const p_id );       
        virtual bool setPanId( const panId_t p_id );       

        virtual bool getSourceAddress( uint16_t* const p_addr );       
        virtual bool setSourceAddress( const uint16_t p_addr );       

        virtual bool getRetries( uint8_t* const p_addr );       
        virtual bool setRetries( const uint8_t p_addr );       

        virtual bool getRandomDelaySlots( uint8_t* const p_addr );       
        virtual bool setRandomDelaySlots( const uint8_t p_addr );       

        virtual bool getMacMode( XBeeApiMACMode_e* const p_mode );       
        virtual bool setMacMode( const XBeeApiMACMode_e p_mode );       
};

/** Class to access the configuration interface of the XBee.
    In contrast to XBeeApiCmdAt, the getXXX methods block
    until the data is received (or a timeout has occurred)
    which means that the caller doesn't have to deal with the
    asynchronous nature of the API provided by XBeeApiCmdAt.
    
    It's not necessary to use any of the requestXXX methods
    (as the getXXX methods will take care of this, however
    calling a requestXXX method will effectively pre-fetch the
    data meaning that getXXX will not have to block */
class XBeeApiCmdAtBlocking : public XBeeApiCmdAt
{
    protected:
        /** Timeout used for blocking methods in milliseconds */
        uint16_t m_timeout;
        
        /** Wait slice time while blocking.  The function will 
            wait_ms(m_slice) until the XBee responds with the
            data or m_timeout elapses */
        uint16_t m_slice;

    public:
       /** Constructor 
       
            \param p_device XBee device with which this object should 
                            be associated 
            \param p_timeout Timeout to be used when waiting for 
                             data from the XBee, specified in
                             milliseconds
            \param p_slice While waiting for data, blocking methods
                           will call the OS wait_ms() function, using
                           the value specified by p_slice */
       XBeeApiCmdAtBlocking( XBeeDevice* const p_device = NULL,
                            const uint16_t p_timeout = 1000, 
                            const uint16_t p_slice = 100);
       
       /** Destructor */
       virtual ~XBeeApiCmdAtBlocking( void ) {};
 
       /* Implement XBeeApiCmdAt's virtual methods */
       
       virtual bool getHardwareVersion( uint16_t* const p_ver );
       virtual bool getFirmwareVersion( uint16_t* const p_ver );
        virtual bool getSerialNumber( uint64_t* const p_sn );

       virtual bool getChannel( uint8_t* const p_chan );
       virtual bool setChannel( uint8_t const p_chan );
        
       virtual bool getCoordinatorEnabled( bool* constp_en );
       virtual bool setCoordinatorEnabled( const bool p_en );
       
       virtual bool getEndDeviceAssociationEnabled( bool* const p_en ); 
       virtual bool setEndDeviceAssociationEnabled( const bool p_en  );
       
       virtual bool getPanId( panId_t* const p_id );       
       virtual bool setPanId( const panId_t p_id );

       virtual bool getSourceAddress( uint16_t* const p_addr );       
       virtual bool setSourceAddress( const uint16_t p_addr );  
       
        virtual bool getRetries( uint8_t* const p_addr );       
        virtual bool setRetries( const uint8_t p_addr );       

        virtual bool getRandomDelaySlots( uint8_t* const p_addr );       
        virtual bool setRandomDelaySlots( const uint8_t p_addr );       

        virtual bool getMacMode( XBeeApiMACMode_e* const p_mode );       
        virtual bool setMacMode( const XBeeApiMACMode_e p_mode );       
};

#endif