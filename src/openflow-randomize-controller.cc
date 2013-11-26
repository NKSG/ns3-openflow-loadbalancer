#include "openflow-controller.h"
#include "ns3/random-variable.h"

namespace ns3 {

namespace ofi {

void RandomizeController::ReceiveFromSwitch (Ptr<OpenFlowSwitchNetDevice> swtch, ofpbuf* buffer) {
  if (m_switches.find (swtch) == m_switches.end ())
  {
      NS_LOG_ERROR ("Can't receive from this switch, not registered to the Controller.");
      return;
  }

  // We have received any packet at this point, so we pull the header to figure out what type of packet we're handling.
  uint8_t type = GetPacketType (buffer);

  if (type == OFPT_PACKET_IN) // The switch didn't understand the packet it received, so it forwarded it to the controller.
  {
      ofp_packet_in * opi = (ofp_packet_in*)ofpbuf_try_pull (buffer, offsetof (ofp_packet_in, data));
      int port = ntohs (opi->in_port);

      // Create matching key.
      sw_flow_key key;
      key.wildcards = 0;
      flow_extract (buffer, port != -1 ? port : OFPP_NONE, &key.flow);

      // out put port
      uint16_t out_port = OFPP_FLOOD;

      // for non broadcasting packet, randomly choose a port to send.
      Mac48Address dst_addr;
      dst_addr.CopyFrom (key.flow.dl_dst);
      if (!dst_addr.IsBroadcast ())
      {
           uint32_t min = 0;
           uint32_t max = 10; //TODO: should get port number from switch itself
           UniformVariable uv;

           /*The values returned by a uniformly distributed random
           variable should always be within the range
           [min, max)  */
          out_port = x->GetInteger(min, max);
      }
      else
      {
          NS_LOG_INFO ("Setting to flood; this packet is a broadcast");
      }

     // Create output-to-port action
      ofp_action_output x[1];
      x[0].type = htons (OFPAT_OUTPUT);
      x[0].len = htons (sizeof(ofp_action_output));
      x[0].port = out_port;

      // Create a new flow that outputs is random.
      ofp_flow_mod* ofm = BuildFlow (key, opi->buffer_id, OFPFC_ADD, x, sizeof(x), OFP_FLOW_PERMANENT, OFP_FLOW_PERMANENT);
      SendToSwitch (swtch, ofm, ofm->header.length);
  }

  return;
}

}

}
