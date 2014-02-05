/**
   @file
   @brief Class inheriting from XBeeApiTxFrame with some useful but 
          non-essential function.
      
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

#if !defined XBEEAPITXFRAMEEX_HPP
#define      XBEEAPITXFRAMEEX_HPP

#include "XBeeApiTxFrame.hpp"

#include <stdint.h>

/** Class which extends XBeeApiTxFrame with a number of additional useful
    features.  These features are partitioned within this sub-class so
    that other classes can inherit from XBeeApiTxFrame without also aquiring
    this "baggage".
*/
class XBeeApiTxFrameEx : public XBeeApiTxFrame
{
    protected:
        /** Keep stats on the various TX confirmations received from the XBee */
        uint16_t m_statusCounters[ XBeeApiTxFrame::XBEE_API_TX_STATUS_LAST ];
        
        /** Status of the most recent TX confirmation */
        XBeeApiTxStatus_e m_recent;
        
    public:
        /** Constuctor - see XBeeApiTxFrame constructor */
        XBeeApiTxFrameEx( XBeeDevice* p_device = NULL );
        
        /** Destructor */
        virtual ~XBeeApiTxFrameEx();
        
        /** Callback which is invoked when a response to a TX frame is received.
            The implementation in this class simply updates m_statusCounters to
            keep stats on the result of the TX attempts 
            
            \param p_status The status of the TX attempt */
        virtual void frameTxCallback( const XBeeApiTxStatus_e p_status );
        
        /** Method to retrieve the number of TX attempts which have had the
            specified status result.  Simply an accessor to m_statusCounters.
            
            \param p_status Status to look-up
        */
        uint16_t getStatusCount( const XBeeApiTxStatus_e p_status );
        
        /** Return the most recent status, as informed to the object by a 
            frameTxCallback invokation.  
            
            \returns Most recent TX status.  Will return 
                    XBeeApiTxFrame::XBEE_API_TX_STATUS_LAST in the case that 
                    there have not been any callbacks yet. */
        XBeeApiTxFrame::XBeeApiTxStatus_e getMostRecentStatus( void ) const;
};

#endif