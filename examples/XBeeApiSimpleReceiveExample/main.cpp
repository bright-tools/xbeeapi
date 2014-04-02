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

/** Circular buffer to receive the data frames from the XBee.  Any frames
    that won't fit in the buffer when they are received will be lost */
XBeeApiRxFrameCircularBuffer rxBuffer( 10, &xbeeDevice );
  
void dumpFrame( const XBeeApiRxFrame* const p_frame )
{
    const uint8_t* outData;
    uint16_t outLoop;
    pc.printf(" API ID: 0x%02X\r\n",p_frame->getApiId() );
    p_frame->getDataPtr( 0, &outData, &outLoop );
    pc.printf("Data [%d]: ",outLoop);
    for( ;
        outLoop > 0;
	outLoop--,outData++ )
    {
        pc.printf("0x%02X ",*outData);
    }
    pc.printf("\r\n");
}

int main() {
    /* This example will use the blocking API for simplicity */   
    XBeeApiCmdAtBlocking atIf( &xbeeDevice );

    XBeeDevice::XBeeDeviceReturn_t status;

    /* Get API mode 2 set up - this is a pre-requisit to using other XBeeApi functions.
       This step may not be needed in the case that the XBee has already been configured
       to use Mode 2 and the setting has been stored in NV */
    status = xbeeDevice.setUpApi();

    if( status == XBeeDevice::XBEEDEVICE_OK )
    {
        /* Set up a peer-to-peer network using the specified PAN and channel */
        if( xbeeSetNetworkTypeP2P( &atIf, myPANId, myChannelId ) )
	{
		const XBeeApiRxFrame* frameP;
		while( 1 )
		{
			/* Wait for a while, report how many frames are in the buffer then clear
			   clear the buffer out for the next loop around */
			wait(10);

			pc.printf("Received frames: %d\r\n",rxBuffer.getFrameCount() );

			/* Work through all of the frames in the buffer */
			while( NULL != ( frameP = rxBuffer.getTailPtr()))
			{
                            /* Dump out some information from the frame */
			    dumpFrame( frameP );
			    /* Remove the frame from the buffer */
                            rxBuffer.pop();
			}	
		}
	}
	else
	{
    		pc.printf("xbeeSetNetworkTypeP2P failed\r\n");
	}
    }
    else
    {
    	pc.printf("setUpApi failed with status %d\r\n",status);
    }
}
