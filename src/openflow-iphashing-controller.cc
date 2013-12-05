#include "openflow-controller.h"
#include "ipv4-address.h"
#include "ns3/log.h"

#include <iostream>
#include <string>

namespace ns3 {

    namespace ofi {

        NS_LOG_COMPONENT_DEFINE("IPHashingController");
        
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
                flow_extract (buffer, port != －1 ? port : OFPP_NONE, &key.flow);
                
                uint16_t out_port = OFPP_FLOOD;
                uint16_t in_port = OFPP_NONE;
                
                uint32_t flow_dst_nw_addr = 0;
                uint32_t flow_src_nw_addr = 0;
                
                Ipv4Address dst_addr;
                Ipv4Address src_addr;
                Ipv4Address server_addr ("10.1.1.254");
                dst_addr.ConvertFrom(key.flow.nw_dst);
                flow_dst_nw_addr=key.flow.nw_dst;
                flow_src_nw_addr=key.flow.nw_src;
                
                if (!dst_addr.isBroadcast()) {
                    
                }
                
                else {
                    out_port = OFPP_FLOOD;
                }
                
                //create output-to-port action
                ofp_action_output x[1];
                x[0].type = htons (OFPAT_OUTPUT);
                x[0].len = htons (sizeof(ofp_action_output));
                x[0].port = out_port;
                NS_LOG_FUNCTION (out_port);
                ofp_flow_mod* ofm = BuildFlow(key, opi->buffer_id, OFPFC_ADD, x, sizeof(x), OFP_FLOW_PERMANENT, OFP_FLOW_PERMANENT);
                SendToSwitch(swtch, ofm, ofm->header.length);
            }
            
            return;
        }

    }

}





















