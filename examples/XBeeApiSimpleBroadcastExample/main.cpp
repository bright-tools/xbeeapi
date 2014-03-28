/**
   @file
   @brief Example of using the XBeeApi library to broadcast a message
          This example has a minimum of error checking in order
          to keep the code compact

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

#include "mbed.h"   
#include "xbeeapi.hpp"

Serial pc(USBTX, USBRX); // tx, rx

const uint8_t tx_data[] = { 'H', 'E', 'L', 'L', 'O' };

/* TODO: You may need to change these based on the device/connections that you're using */ 
#define XBEE_TX_PIN PTA2
#define XBEE_RX_PIN PTA1

/* Network address for our XBee */
const uint16_t myNetworkAddress = 0x1234;

/* ID for the Personal Area Network we're going to join */
const XBeeApiCmdAt::panId_t myPANId = 1000;

/* Network channel to use */
const XBeeApiCmdAt::channel_t myChannelId = 14;

/* This example sends a frame to a specific destination address - we could set
   the frame to be broadcast by calling the setDestAddrBroadcast() method
   instead of setDestAddr() in the function below.
   By default 16-bit addressing mode is used.  setDestAddrType can be used
   to select between 16-bit and 64-bit addressing modes */
const uint16_t frameDestinationAddress = 0x01;

XBeeDevice xbeeDevice( XBEE_TX_PIN, XBEE_RX_PIN, NC, NC );
   
int main() {
    /* This example will use the blocking API for simplicity */   
    XBeeApiCmdAtBlocking atIf( &xbeeDevice );

    //pc.printf("XBeeApiSimpleBroadcastExample\r\n");

    XBeeDevice::XBeeDeviceReturn_t status;

    /* This is the frame we're going to transmit */
    XBeeApiTxFrame frame( &xbeeDevice );
        
    /* Get API mode 2 set up - this is a pre-requisit to using other XBeeApi functions.
       This step may not be needed in the case that the XBee has already been configured
       to use Mode 2 and the setting has been stored in NV */
    status = xbeeDevice.setUpApi();

    if( status == XBeeDevice::XBEEDEVICE_OK )
    {
        /* Set the 16-bit source address of this XBee */
        atIf.setSourceAddress( myNetworkAddress );
        /* Set up a peer-to-peer network using the specified PAN and channel */
        xbeeSetNetworkTypeP2P( &atIf, myPANId, myChannelId );

        /* Set the data pointer & destination address in the transmit frame */
        frame.setDataPtr( tx_data, sizeof( tx_data ) );
        frame.setDestAddr( frameDestinationAddress );
       
        while( 1 )
	{	
        	xbeeDevice.SendFrame( &frame );
		wait(1);
	}	
    }
    else
    {
    	pc.printf("setUpApi failed with status %d\r\n",status);
    }
}
