#include "openflow-controller.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/openflow-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/ipv4-address.h"

#include <iostream>
#include <fstream>
#include <string>

namespace ns3 {

    namespace ofi {

        void IPHashingController::ReceiveFromSwitch (Ptr<OpenFlowSwitchNetDevice> swtch,
                                                     ofpbuf* buffer) {
            if (m_switches.find(swtch) == m_switches.end())  {
                NS_LOG_ERROR("Can't send to this switch, not registered to the Controller.");
                return;
            }
            
            //Received any packet at this point, so we pull the header to figure out what type of packet we are handing
            uint8_t type = GetPacketType(buffer);
            
            if (type == OFPT_PACKET_IN) {
                ofp_packet_in * opi = (ofp_packet_in*)ofpbuf_try_pull (buffer, offsetof
                                                                       (ofp_packet_in, data));
                int port = ntohs (opi->in_port);
                
                //Matching key
                sw_flow_key key;
                key.wildcards = 0;
                flow_extract (buffer, port != 1 ? port : OFPP_NONE, &key.flow);
                
                uint16_t out_port = OFPP_FLOOD;
                uint16_t in_port = ntohs (key.flow.in_port);
                
                Ipv4Address dst_addr;
                dst_addr.ConvertFrom(key.flow.dl_dst);
                
                if (!dst_addr.isBroadcast()){
                    out_port = st->second.port;
                }
                
                else{
                    NS_LOG_INFO ("Setting to flood; don't know yet what port " << dst_addr " is connected ");
                }
                
                
                //Create output-to-port action
                
                ofp_action_output x[1];
                x[0].type = htons (PFPAT_OUTPUT);
                x[0].len = htons (sizeof(ofp_action_output));
                x[0].port = out_port;
                
                ofp_flow_mod* ofm = BuildFlow (key, opi->buffer_id, OFPFC_ADD, x, sizeof(x), OFP_FLOW_PERMANENT, m-m_expirationTime.IsZero () ? OFP_FLOW_PERMANENT : m_expirationTime.GetSeconds ());
                SendToSwitch (swtch, ofm, ofm->header.length);
                
                Ipv4Address src_addr;
                src_addr.ConvertFrom (key.flow.dl_src);
                LearnState_t::iterator st = m_learnState.find (src_addr);
                if (st == m_learnState.end()) {
                    LearnState ls;
                    
                }
            }
            
            return;
        }

    }

}





















