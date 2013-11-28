#include "openflow-controller.h"
#include "openflow-loadbalancer.h"

#include "ns3/random-variable.h"
#include "ns3/log.h"

#include <iostream>
#include <fstream>

namespace ns3 {

namespace ofi {

void RandomizeController::ReceiveFromSwitch (Ptr<OpenFlowSwitchNetDevice> swtch, ofpbuf* buffer) {
  if (m_switches.find (swtch) == m_switches.end ())
  {
      //NS_LOG_ERROR ("Can't receive from this switch, not registered to the Controller.");
      return;
  }

  // We have received any packet at this point, so we pull the header to figure out what type of packet we're handling.
  uint8_t type = GetPacketType (buffer);

  if (type == OFPT_PACKET_IN) // The switch didn't understand the packet it received, so it forwarded it to the controller.
  {
      ofp_packet_in * opi = (ofp_packet_in*)ofpbuf_try_pull (buffer, offsetof (ofp_packet_in, data));
      int port = ntohs (opi->in_port);/* Port on which frame was received. */
      
      uint16_t out_port = OFPP_FLOOD;// out put port
      uint16_t in_port = OFPP_NONE;
	  
      uint32_t flow_dst_nw_addr=0;;
      uint32_t flow_src_nw_addr=0;
      Ipv4Address server_addr ("10.1.1.254");

      uint32_t min = 0;
      uint32_t max = OF_DEFAULT_SERVER_NUMBER;
      UniformVariable uv;
      
      sw_flow_key key;
	  
      key.wildcards = 0;// Create matching key.
      flow_extract(buffer, port != -1 ? port : OFPP_NONE, &key.flow);
      in_port = ntohs (key.flow.in_port);
      flow_dst_nw_addr=key.flow.nw_dst;
	  flow_src_nw_addr=key.flow.nw_src;
	  
      //compare flow dest ip address with server ip address key.flow.nw_dst
      if(server_addr.Get()==flow_dst_nw_addr)//flow is from client to server, randomly select a port to forward.
        {
            out_port = uv.GetInteger(min, max);
      
            // Create output-to-port action
            ofp_action_output x[1];
            x[0].type = htons (OFPAT_OUTPUT);
            x[0].len = htons (sizeof(ofp_action_output));
            x[0].port = out_port;
      
            // Create a new flow from src to dst that output port is random.
            ofp_flow_mod* ofm = BuildFlow (key, opi->buffer_id, OFPFC_ADD, x, sizeof(x), OFP_FLOW_PERMANENT, OFP_FLOW_PERMANENT);
            SendToSwitch (swtch, ofm, ofm->header.length);

            //record client in_port with client ip so future flow with this ip as dst will be directed to this in_port
            PortRecord_t::iterator st = m_portrecord.find (flow_src_nw_addr);
            if (st == m_portrecord.end ())
            {
               PortRecord pr;
               pr.port = in_port;
               m_portrecord.insert (std::make_pair (flow_src_nw_addr,pr));
            }
        }
      else//flow is from server to client, look up recorded table to direct flow to client ip/port
        {
            PortRecord_t::iterator st = m_portrecord.find (flow_dst_nw_addr);
            if (st != m_portrecord.end ())
            {
                out_port = st->second.port;//if client ip and port are recorded
            }
            else//else flood 
            {
                out_port=OFPP_FLOOD;
            }
                
            // Create output-to-port action
            ofp_action_output x[1];
            x[0].type = htons (OFPAT_OUTPUT);
            x[0].len = htons (sizeof(ofp_action_output));
            x[0].port = out_port;
            // Create a new flow from src to dst that output port is random.
            ofp_flow_mod* ofm = BuildFlow (key, opi->buffer_id, OFPFC_ADD, x, sizeof(x), OFP_FLOW_PERMANENT, OFP_FLOW_PERMANENT);
            SendToSwitch (swtch, ofm, ofm->header.length);
        }
  }

  return;
}

}

}
