#ifndef OPENFLOW_CONTROLLER_H
#define OPENFLOW_CONTROLLER_H

#include "ns3/openflow-interface.h"
#include "ns3/openflow-switch-net-device.h"

namespace ns3 {

namespace ofi {

class RandomizeController : public Controller {
 public:
  void ReceiveFromSwitch (Ptr<OpenFlowSwitchNetDevice> swtch, ofpbuf* buffer);
};

class RoundRobinController : public Controller {
 public:
  void ReceiveFromSwitch (Ptr<OpenFlowSwitchNetDevice> swtch, ofpbuf* buffer);
};

class IPHashingController : public Controller {
 public:
  void ReceiveFromSwitch (Ptr<OpenFlowSwitchNetDevice> swtch, ofpbuf* buffer);
};

}

}
#endif
