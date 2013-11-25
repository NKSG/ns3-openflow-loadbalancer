#include "openflow-controller.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/openflow-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

namespace ns3 {

namespace ofi {

void IPHashingController::ReceiveFromSwitch (Ptr<OpenFlowSwitchNetDevice> swtch, ofpbuf* buffer) {
  return;
}

}

}
