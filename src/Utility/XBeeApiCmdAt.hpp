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
    
    protected:
        /** Indicates whether or not m_hwVer contains data retrieved from the XBee */
        bool m_haveHwVer;
        /** Indicates whether or not m_fwVer contains data retrieved from the XBee */
        bool m_haveFwVer;
        /** Indicates whether or not m_chan contains data retrieved from the XBee */
        bool m_haveChan;
        /** Indicates whether or not m_PANId contains data retrieved from the XBee */
        bool m_havePANId;
        /** Indicates whether or not m_EDA contains data retrieved from the XBee */
        bool m_haveEDA;
        /** Indicates whether or not m_CE contains data retrieved from the XBee */
        bool m_haveCE;
        
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

        /** Constructor */
        XBeeApiCmdAt( );
       
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
       
       virtual bool getChannel( uint8_t* const p_chan );
       virtual bool setChannel( uint8_t const p_chan );
       
       virtual bool getCoordinatorEnabled( bool* constp_en );
       virtual bool setCoordinatorEnabled( const bool p_en );
       
       virtual bool getEndDeviceAssociationEnabled( bool* const p_en ); 
       virtual bool setEndDeviceAssociationEnabled( const bool p_en  );
       
       virtual bool getPanId( panId_t* const p_id );       
       virtual bool setPanId( const panId_t p_id );       
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
       
            \param p_timeout Timeout to be used when waiting for 
                             data from the XBee, specified in
                             milliseconds
            \param p_slice While waiting for data, blocking methods
                           will call the OS wait_ms() function, using
                           the value specified by p_slice */
       XBeeApiCmdAtBlocking( const uint16_t p_timeout = 1000, const uint16_t p_slice = 100);
       
       /** Destructor */
       virtual ~XBeeApiCmdAtBlocking( void ) {};
 
       /* Implement XBeeApiCmdAt's virtual methods */
       
       virtual bool getHardwareVersion( uint16_t* const p_ver );
       virtual bool getFirmwareVersion( uint16_t* const p_ver );

       virtual bool getChannel( uint8_t* const p_chan );
       virtual bool setChannel( uint8_t const p_chan );
        
       virtual bool getCoordinatorEnabled( bool* constp_en );
       virtual bool setCoordinatorEnabled( const bool p_en );
       
       virtual bool getEndDeviceAssociationEnabled( bool* const p_en ); 
       virtual bool setEndDeviceAssociationEnabled( const bool p_en  );
       
       virtual bool getPanId( panId_t* const p_id );       
       virtual bool setPanId( const panId_t p_id );
};

#endif