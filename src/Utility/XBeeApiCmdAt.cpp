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

#include "XBeeApiCmdAt.hpp"

/* Set of Frame ID codes for the various commands (see XBEE_CMD_POSN_FRAME_ID) */

#define CMD_RESPONSE_GET_VR  '1'
#define CMD_RESPONSE_GET_HV  '2'
#define CMD_RESPONSE_GET_CH  '3'
#define CMD_RESPONSE_SET_CH  '4'
#define CMD_RESPONSE_SET_CE  '5'
#define CMD_RESPONSE_GET_CE  '6'
#define CMD_RESPONSE_SET_EDA '7'
#define CMD_RESPONSE_GET_EDA '8'
#define CMD_RESPONSE_SET_PID '9'
#define CMD_RESPONSE_GET_PID '0'
#define CMD_RESPONSE_SET_MY  'a'
#define CMD_RESPONSE_GET_MY  'b'
#define CMD_RESPONSE_GET_SH  'c'
#define CMD_RESPONSE_GET_SL  'd'
#define CMD_RESPONSE_SET_RR  'e'
#define CMD_RESPONSE_GET_RR  'f'
#define CMD_RESPONSE_SET_RN  'g'
#define CMD_RESPONSE_GET_RN  'h'
#define CMD_RESPONSE_SET_MM  'i'
#define CMD_RESPONSE_GET_MM  'j'

/** Lowest channel supported by the XBee S1 */
#define XBEE_CHAN_MIN 0x0b
/** Highest channel supported by the XBee S1 */
#define XBEE_CHAN_MAX 0x1a

/** Lowest channel supported by the XBee S1 Pro */
#define XBEE_PRO_CHAN_MIN 0x0c
/** Highest channel supported by the XBee S1 Pro */
#define XBEE_PRO_CHAN_MAX 0x17

/* Content for the various commands - value of 0 indicates a value to be populated (i.e. variable) */

static const uint8_t cmd_vr[] =      { CMD_RESPONSE_GET_VR, 'V', 'R' };
static const uint8_t cmd_hv[] =      { CMD_RESPONSE_GET_HV, 'H', 'V' };
static const uint8_t cmd_sh[] =      { CMD_RESPONSE_GET_SH, 'S', 'H' };
static const uint8_t cmd_sl[] =      { CMD_RESPONSE_GET_SL, 'S', 'L' };

static const uint8_t cmd_ch[] =      { CMD_RESPONSE_GET_CH, 'C', 'H' };
static const uint8_t cmd_set_ch[] =  { CMD_RESPONSE_SET_CH, 'C', 'H', 0 };

static const uint8_t cmd_ce[] =      { CMD_RESPONSE_GET_CE, 'C', 'E' };
static const uint8_t cmd_set_ce[] =  { CMD_RESPONSE_SET_CE, 'C', 'E', 0 };

static const uint8_t cmd_eda[] =     { CMD_RESPONSE_GET_EDA, 'A', '1' };
static const uint8_t cmd_set_eda[] = { CMD_RESPONSE_SET_EDA, 'A', '1', 0 };

static const uint8_t cmd_pid[] =     { CMD_RESPONSE_GET_PID, 'I', 'D' };
static const uint8_t cmd_set_pid[] = { CMD_RESPONSE_SET_PID, 'I', 'D', 0, 0 };

static const uint8_t cmd_my[] =      { CMD_RESPONSE_GET_MY,  'M', 'Y' };
static const uint8_t cmd_set_my[] =  { CMD_RESPONSE_SET_MY,  'M', 'Y', 0, 0 };

static const uint8_t cmd_rr[] =      { CMD_RESPONSE_GET_RR,  'R', 'R' };
static const uint8_t cmd_set_rr[] =  { CMD_RESPONSE_SET_RR,  'R', 'R', 0 };

static const uint8_t cmd_rn[] =      { CMD_RESPONSE_GET_RN,  'R', 'N' };
static const uint8_t cmd_set_rn[] =  { CMD_RESPONSE_SET_RN,  'R', 'N', 0 };

static const uint8_t cmd_mm[] =      { CMD_RESPONSE_GET_MM,  'M', 'M' };
static const uint8_t cmd_set_mm[] =  { CMD_RESPONSE_SET_MM,  'M', 'M', 0 };

#define XBEE_CMD_POSN_FRAME_ID (4U)
#define XBEE_CMD_POSN_STATUS (7U)
#define XBEE_CMD_POSN_PARAM_START (8U)

#define XBEE_CMD_RESPONS_HAS_DATA( _p_len ) ((_p_len > ( XBEE_CMD_POSN_PARAM_START + 1 ))

XBeeApiCmdAt::XBeeApiCmdAt( XBeeDevice* const p_device ) : XBeeApiFrameDecoder( p_device ) , 
    m_have_hwVer( false ),
    m_have_fwVer( false ),
    m_have_chan( false ),
    m_have_PANId( false ),
    m_have_EDA( false ),
    m_have_CE( false ),
    m_have_sourceAddress( false ),
    m_have_snLow( false ),
    m_have_snHigh( false ),
    m_have_retries( false ),
    m_have_randomDelaySlots( false ),
    m_have_macMode( false )
{
}

#define PROCESS_SET_GET_RESPONSE_GENERIC( _type, _var, _src, _t ) \
            case CMD_RESPONSE_SET_ ## _type: \
            case CMD_RESPONSE_GET_ ## _type: \
                if( p_data[ XBEE_CMD_POSN_STATUS ] == 0 ) \
                { \
                    if( CMD_RESPONSE_GET_ ## _type == p_data[ XBEE_CMD_POSN_API_ID ] ) \
                    { \
                        m_ ##_var = (_t) (_src); \
                    } \
                    else \
                    { \
                        m_ ## _var = m_ ## _var ## Pend; \
                    } \
                    m_have_ ## _var = true; \
                } \
                else \
                { \
                    /* TODO */ \
                } \
                ret_val = true; \
                break;

#define PROCESS_GET_RESPONSE_GENERIC( _type, _var, _src ) \
            case CMD_RESPONSE_GET_ ## _type: \
                if( p_data[ XBEE_CMD_POSN_STATUS ] == 0 ) \
                { \
                    m_ ##_var = _src; \
                    m_have_ ## _var = true; \
                } \
                else \
                { \
                    /* TODO */ \
                } \
                ret_val = true; \
                break;

#define PROCESS_SET_GET_RESPONSE_8BIT_WITHCAST( _type, _var, _t )  PROCESS_SET_GET_RESPONSE_GENERIC( _type, _var, p_data[ XBEE_CMD_POSN_PARAM_START ], _t )
#define PROCESS_SET_GET_RESPONSE_8BIT( _type, _var )               PROCESS_SET_GET_RESPONSE_GENERIC( _type, _var, p_data[ XBEE_CMD_POSN_PARAM_START ], uint8_t )
#define PROCESS_SET_GET_RESPONSE_16BIT( _type, _var )              PROCESS_SET_GET_RESPONSE_GENERIC( _type, _var, ((uint16_t)p_data[ XBEE_CMD_POSN_PARAM_START ] << 8) | p_data[ XBEE_CMD_POSN_PARAM_START + 1 ], uint16_t )

#define PROCESS_GET_RESPONSE_16BIT( _type, _var ) PROCESS_GET_RESPONSE_GENERIC( _type, _var, ((uint16_t)p_data[ XBEE_CMD_POSN_PARAM_START ] << 8) | p_data[ XBEE_CMD_POSN_PARAM_START + 1 ] )
#define PROCESS_GET_RESPONSE_32BIT( _type, _var ) PROCESS_GET_RESPONSE_GENERIC( _type, _var, ((uint32_t)p_data[ XBEE_CMD_POSN_PARAM_START ] << 24) |\
                                                                                             ((uint32_t)p_data[ XBEE_CMD_POSN_PARAM_START + 1 ] << 16) |\
                                                                                             ((uint32_t)p_data[ XBEE_CMD_POSN_PARAM_START + 2 ] << 8) |\
                                                                                             ((uint32_t)p_data[ XBEE_CMD_POSN_PARAM_START + 3 ]))


bool XBeeApiCmdAt::decodeCallback( const uint8_t* const p_data, size_t p_len )
{
    bool ret_val = false;

    if( XBEE_CMD_AT_RESPONSE == p_data[ XBEE_CMD_POSN_API_ID ] ) {

        switch( p_data[ XBEE_CMD_POSN_FRAME_ID ] ) {
            
            PROCESS_GET_RESPONSE_16BIT( HV, hwVer )
            PROCESS_GET_RESPONSE_16BIT( VR, fwVer )
            
            PROCESS_SET_GET_RESPONSE_8BIT( CH, chan )
            PROCESS_SET_GET_RESPONSE_8BIT( CE, CE )
            PROCESS_SET_GET_RESPONSE_8BIT( PID, PANId )
            PROCESS_SET_GET_RESPONSE_8BIT( EDA, EDA )
            PROCESS_SET_GET_RESPONSE_8BIT( RR, retries )
            PROCESS_SET_GET_RESPONSE_16BIT( MY, sourceAddress )
            PROCESS_GET_RESPONSE_32BIT( SH, snHigh )
            PROCESS_GET_RESPONSE_32BIT( SL, snLow )
            PROCESS_SET_GET_RESPONSE_8BIT( RN, randomDelaySlots )
            PROCESS_SET_GET_RESPONSE_8BIT_WITHCAST( MM, macMode, XBeeApiMACMode_e )
        }
    }
    return ret_val;
}

bool XBeeApiCmdAt::setChannel( uint8_t const p_chan )
{
    bool ret_val = false;
    
    if((( m_device->getXBeeModel() == XBeeDevice::XBEEDEVICE_S1 ) && 
        ( p_chan >= XBEE_CHAN_MIN ) &&
        ( p_chan <= XBEE_CHAN_MAX )) ||
       (( m_device->getXBeeModel() == XBeeDevice::XBEEDEVICE_S1_PRO ) && 
        ( p_chan >= XBEE_PRO_CHAN_MIN ) &&
        ( p_chan <= XBEE_PRO_CHAN_MAX )))
    {
        XBeeApiCmdAtSet<uint8_t> req( cmd_set_ch, p_chan );
    
        m_chanPend = p_chan;
        m_device->SendFrame( &req );
        ret_val = true;
    }
    return ret_val;
}

#define MAKE_REQUEST( _name, _mnemonic, _cmd ) \
bool XBeeApiCmdAt::request ## _name( void ) \
{\
    XBeeApiFrame req( XBEE_CMD_AT_CMD, _cmd, sizeof( _cmd ));\
    m_have_ ## _mnemonic = false;\
    m_device->SendFrame( &req );\
    return true;\
}

MAKE_REQUEST( HardwareVersion, hwVer, cmd_hv )
MAKE_REQUEST( FirmwareVersion, fwVer, cmd_vr )
MAKE_REQUEST( Channel, chan, cmd_ch )
MAKE_REQUEST( PanId, PANId, cmd_pid )
MAKE_REQUEST( CoordinatorEnabled, CE, cmd_ce )
MAKE_REQUEST( EndDeviceAssociationEnabled, EDA, cmd_eda )
MAKE_REQUEST( SourceAddress, sourceAddress, cmd_my )
MAKE_REQUEST( Retries, retries, cmd_rr )
MAKE_REQUEST( RandomDelaySlots, randomDelaySlots, cmd_rn )
MAKE_REQUEST( MacMode, macMode, cmd_mm );

bool XBeeApiCmdAt::requestSerialNumber( void )
{
    XBeeApiFrame req1( XBEE_CMD_AT_CMD, cmd_sh, sizeof( cmd_sh ));
    XBeeApiFrame req2( XBEE_CMD_AT_CMD, cmd_sl, sizeof( cmd_sl ));
    m_have_snHigh = m_have_snLow = false;
    m_device->SendFrame( &req1 );
    m_device->SendFrame( &req2 );
    return true;
}

#define MAKE_GET(_name, _mnemonic, _type ) \
bool XBeeApiCmdAt::get ## _name( _type* const p_param ) \
{\
    if( m_have_ ## _mnemonic ) {\
        *p_param = m_ ## _mnemonic;\
    } \
    return m_have_ ## _mnemonic; \
}

MAKE_GET( FirmwareVersion, fwVer, uint16_t )
MAKE_GET( HardwareVersion, hwVer, uint16_t )
MAKE_GET( Channel, chan, uint8_t )
MAKE_GET( CoordinatorEnabled, CE, bool );
MAKE_GET( EndDeviceAssociationEnabled, EDA, bool )
MAKE_GET( PanId, PANId, panId_t )
MAKE_GET( SourceAddress, sourceAddress, uint16_t )
MAKE_GET( Retries, retries, uint8_t )
MAKE_GET( RandomDelaySlots, randomDelaySlots, uint8_t )
MAKE_GET( MacMode, macMode, XBeeApiMACMode_e )

bool XBeeApiCmdAt::getSerialNumber( uint64_t* const p_sn )
{
    /* Need both halves to have the complete serial number */
    bool have_sn = m_have_snLow && m_have_snHigh;
    if( have_sn )
    {
        *p_sn = m_snHigh;
        *p_sn = *p_sn << 32U;
        *p_sn |= m_snLow;
    }
    return( have_sn );
}

#define MAKE_SET( _name, _mnemonic, _cmd, _type ) \
bool XBeeApiCmdAt::set ## _name( const _type p_param ) \
{\
    XBeeApiCmdAtSet<_type> req( _cmd, p_param );\
\
    m_have_ ## _mnemonic = false;\
    m_## _mnemonic ## Pend = p_param;\
    m_device->SendFrame( &req );\
    return true;\
}

MAKE_SET( CoordinatorEnabled,          CE,               cmd_set_ce,  bool )
MAKE_SET( EndDeviceAssociationEnabled, EDA,              cmd_set_eda, bool )
MAKE_SET( PanId,                       PANId,            cmd_set_pid, panId_t )
MAKE_SET( SourceAddress,               sourceAddress,    cmd_set_my,  uint16_t )
MAKE_SET( Retries,                     retries,          cmd_set_rr,  uint8_t )
MAKE_SET( RandomDelaySlots,            randomDelaySlots, cmd_set_rn,  uint8_t )
MAKE_SET( MacMode,                     macMode,          cmd_set_mm,  XBeeApiMACMode_e )

XBeeApiCmdAtBlocking::XBeeApiCmdAtBlocking( XBeeDevice* const p_device, const uint16_t p_timeout, const uint16_t p_slice ) :
    XBeeApiCmdAt( p_device ),
    m_timeout( p_timeout ),
    m_slice( p_slice )
{
}

/**
   Macro to wrap around the "requestXXX" & "getXXX" methods and implement a blocking call.
   This macro is used as the basis for getXXX functions in XBeeApiCmdAtBlocking.

   Originally looked to achieve this using a template function passing method pointers, however
   there's no way to get a method pointer to the parent class implementation as opposed to the
   implementation in this class, meaning that the result was a recursive method call.  The joys of
   polymorphism.

   e.g. We pass a pointer to method getHardwareVersion().  The function receiving the pointer
        uses it to make a function call.  The actual function that's called is (correctly)
        the one implemented in this class, however what we actually wanted in this case
        was to call the implementation in the base class.  Using static_cast<> doesn't have
        any effect and taking the address of XBeeApiCmdAt::getHardwareVersion ends up with
        XBeeApiCmdAtBlocking::getHardwareVersion being called due to polymorphism. */
#define BLOCKING_GET( _REQ_FN, _GET_FN, _VAR ) \
    bool ret_val = false; \
\
    if( _GET_FN( _VAR ) )\
    {\
        ret_val = true;\
    } \
    else if( _REQ_FN() )\
    {\
        uint16_t counter = m_timeout; \
        bool cont;\
\
        do{\
            cont = false; \
            wait_ms( m_slice );\
            if( _GET_FN( _VAR ) )\
            {\
                ret_val = true;\
            }\
            else if( counter > m_slice ) {\
                counter -= m_slice;    \
                cont = true;\
            } \
        } while( cont );\
    }\
    \
    return( ret_val );

/**
   Macro to wrap around the "setXXX" & "getXXX" methods and implement a blocking call.
   This macro is used as the basis for setXXX functions in XBeeApiCmdAtBlocking.
*/
#define BLOCKING_SET( _SET_FN, _GET_FN, _VAR, _TYPE ) \
    bool ret_val = false; \
    uint16_t counter = m_timeout; \
    _TYPE readback; \
\
    if( _SET_FN( _VAR ) )\
    {\
        bool cont;\
\
        do{\
            cont = false;\
            wait_ms( m_slice );\
            if( _GET_FN( &readback ) &&\
               ( readback == _VAR ))\
            {\
                ret_val = true;\
            }\
            else if( counter > m_slice ) {\
                counter -= m_slice;    \
                cont = true;\
            } \
        } while( cont );\
    }\
    \
    return( ret_val );


bool XBeeApiCmdAtBlocking::getHardwareVersion( uint16_t* const p_ver )
{
    BLOCKING_GET( XBeeApiCmdAt::requestHardwareVersion,
                  XBeeApiCmdAt::getHardwareVersion,
                  p_ver );
}

bool XBeeApiCmdAtBlocking::getFirmwareVersion( uint16_t* const p_ver )
{
    BLOCKING_GET( XBeeApiCmdAt::requestFirmwareVersion,
                  XBeeApiCmdAt::getFirmwareVersion,
                  p_ver );
}

bool XBeeApiCmdAtBlocking::getSerialNumber( uint64_t* const p_sn )
{
    BLOCKING_GET( XBeeApiCmdAt::requestSerialNumber,
                  XBeeApiCmdAt::getSerialNumber,
                  p_sn );
}

bool XBeeApiCmdAtBlocking::getChannel( uint8_t* const p_chan )
{
    BLOCKING_GET( XBeeApiCmdAt::requestChannel,
                  XBeeApiCmdAt::getChannel,
                  p_chan );
}

bool XBeeApiCmdAtBlocking::setChannel( uint8_t const p_chan )
{
    BLOCKING_SET( XBeeApiCmdAt::setChannel,
                  XBeeApiCmdAt::getChannel,
                  p_chan,
                  uint8_t );
}

bool XBeeApiCmdAtBlocking::getCoordinatorEnabled( bool* const p_en )
{
    BLOCKING_GET( XBeeApiCmdAt::requestCoordinatorEnabled,
                  XBeeApiCmdAt::getCoordinatorEnabled,
                  p_en );
}

bool XBeeApiCmdAtBlocking::setCoordinatorEnabled( const bool p_en )
{
    BLOCKING_SET( XBeeApiCmdAt::setCoordinatorEnabled,
                  XBeeApiCmdAt::getCoordinatorEnabled,
                  p_en,
                  bool );
}

bool XBeeApiCmdAtBlocking::getEndDeviceAssociationEnabled( bool* const p_en )
{
    BLOCKING_GET( XBeeApiCmdAt::requestEndDeviceAssociationEnabled,
                  XBeeApiCmdAt::getEndDeviceAssociationEnabled,
                  p_en );
}

bool XBeeApiCmdAtBlocking::setEndDeviceAssociationEnabled( const bool p_en )
{
    BLOCKING_SET( XBeeApiCmdAt::setEndDeviceAssociationEnabled,
                  XBeeApiCmdAt::getEndDeviceAssociationEnabled,
                  p_en,
                  bool );
}

bool XBeeApiCmdAtBlocking::getPanId( panId_t* const p_chan )
{
    BLOCKING_GET( XBeeApiCmdAt::requestPanId,
                  XBeeApiCmdAt::getPanId,
                  p_chan );
}

bool XBeeApiCmdAtBlocking::setPanId( const panId_t p_chan )
{
    BLOCKING_SET( XBeeApiCmdAt::setPanId,
                  XBeeApiCmdAt::getPanId,
                  p_chan,
                  panId_t );
}

bool XBeeApiCmdAtBlocking::getSourceAddress( uint16_t* const p_addr )
{
    BLOCKING_GET( XBeeApiCmdAt::requestSourceAddress,
                  XBeeApiCmdAt::getSourceAddress,
                  p_addr );
}

bool XBeeApiCmdAtBlocking::setSourceAddress( const uint16_t p_addr )
{
    BLOCKING_SET( XBeeApiCmdAt::setSourceAddress,
                  XBeeApiCmdAt::getSourceAddress,
                  p_addr,
                  uint16_t );
}

bool XBeeApiCmdAtBlocking::getRetries( uint8_t* const p_addr )
{
    BLOCKING_GET( XBeeApiCmdAt::requestRetries,
                  XBeeApiCmdAt::getRetries,
                  p_addr );
}

bool XBeeApiCmdAtBlocking::setRetries( const uint8_t p_addr )
{
    BLOCKING_SET( XBeeApiCmdAt::setRetries,
                  XBeeApiCmdAt::getRetries,
                  p_addr,
                  uint8_t );
}

bool XBeeApiCmdAtBlocking::getRandomDelaySlots( uint8_t* const p_addr )
{
    BLOCKING_GET( XBeeApiCmdAt::requestRandomDelaySlots,
                  XBeeApiCmdAt::getRandomDelaySlots,
                  p_addr );
}

bool XBeeApiCmdAtBlocking::setRandomDelaySlots( const uint8_t p_addr )
{
    BLOCKING_SET( XBeeApiCmdAt::setRandomDelaySlots,
                  XBeeApiCmdAt::getRandomDelaySlots,
                  p_addr,
                  uint8_t );
}

bool XBeeApiCmdAtBlocking::getMacMode( XBeeApiMACMode_e* const p_mode )
{
    BLOCKING_GET( XBeeApiCmdAt::requestMacMode,
                  XBeeApiCmdAt::getMacMode,
                  p_mode );
}
           
bool XBeeApiCmdAtBlocking::setMacMode( const XBeeApiMACMode_e p_mode )
{
    BLOCKING_SET( XBeeApiCmdAt::setMacMode,
                  XBeeApiCmdAt::getMacMode,
                  p_mode,
                  XBeeApiMACMode_e );
}



template < typename T >
XBeeApiCmdAt::XBeeApiCmdAtSet<T>::XBeeApiCmdAtSet( const uint8_t* const p_data,
                                                   const T p_val ) : XBeeApiFrame( )
{
    size_t s;
    uint8_t* dest;
    const uint8_t* src = (uint8_t*)(&p_val);
    
    m_apiId = XBEE_CMD_AT_CMD;
    
    m_buffer[0] = p_data[0];
    m_buffer[1] = p_data[1];
    m_buffer[2] = p_data[2];
    
    m_dataLen = sizeof( m_buffer );

    /* TODO: This copy code isn't portable - it's assuming that the data in 
     * p_data is little endian */
    dest = &( m_buffer[ m_dataLen - 1 ] );
    
    for( s = 0;
         s < sizeof( T );
         s++, dest--, src++ ) {
        *dest = *src;         
    }
    
    m_data = m_buffer;
}

template < typename T >
XBeeApiCmdAt::XBeeApiCmdAtSet<T>::~XBeeApiCmdAtSet()
{
}
