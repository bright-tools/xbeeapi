/**
   @file
   @brief Example of using the XBeeApi library to send AT-style
          commands to change settings in the XBee
 
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
 
/* TODO: You may need to change these based on the device/connections that you're using */ 
#define XBEE_TX_PIN PTA2
#define XBEE_RX_PIN PTA1
 
const uint8_t channelNo = 0xd;
 
Serial pc(USBTX, USBRX); // tx, rx
 
XBeeDevice xbeeDevice( XBEE_TX_PIN, XBEE_RX_PIN, NC, NC );
   
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
        uint16_t fwVersion;
        uint8_t  chan;
        uint8_t  newChan;
        
        /* Ask the XBee for the firmware version it's running.
           
           This function is generally used when employing the non-blocking API in order to
           kick off the process of retrieving the data.  Once the request has been sent
           the method returns and the response is processed asynchronously.  Once received
           the data's available via the getXXX methods.
           
           In the case that the blocking API is used the requestXXX methods are optional
           because the getXXX methods will send the request and then block until the response
           is received.  The main potential advantage to still (selectively) using the requestXXX 
           methods even when employing the blocking API is timing - the getXXX functions do not
           have to communicate with the XBee and can use the previously retrieved data
           meaning that there is no need to block on a communication exchange */
        atIf.requestFirmwareVersion();
        
        /* Attempt to get the previously requested firmware version.   */
        if( atIf.getFirmwareVersion( &fwVersion ) )
        {
            /* Get the channel number.  As noted above, we don't need to use the requestXXX
               in the case that we're using the blocking API */
            if( atIf.getChannel( &chan ) )
            {
                /* Change the channe ID to something new */
                atIf.setChannel( channelNo );
 
                /* Retrieve the channel ID again, after we've changed it */
                atIf.getChannel( &newChan );
                
                pc.printf("Firmware version: 0x%04x\r\nOriginal channel ID 0x%02x, new channel ID 0x%02x",fwVersion,chan,newChan); 
            }
            else
            {
                pc.printf("Retrieval of channel number was unsuccessful\r\n");            
            }
        }
        else
        {
            pc.printf("Retrieval of firmware version was unsuccessful\r\n");            
        }
    }
    else
    {
        pc.printf("setUpApi was unsuccessful - return code %d\r\n", status);
    }
}
