#include <cpplogging/cpplogging.h>
#include <cpputils/SignalManager.h>
#include <dccomms_packets/VariableLength2BPacket.h>

using namespace dccomms;
using namespace std;
using namespace cpputils;
using namespace dccomms_packets;

int main() {
  auto pb = CreateObject<VariableLength2BPacketBuilder>();
  auto node = CreateObject<CommsDeviceService>(pb);

  node->SetCommsDeviceId("bluerov0-dev");
  node->SetLogLevel(warn);
  node->Start();

  Ptr<Logger> log = CreateObject<Logger>();
  log->SetLogName("TX");
  log->FlushLogOn(info);

  auto pkt = pb->Create();

  uint32_t seq = 0;
  while (1) {
    pkt->SetVirtualDestAddr(2);
    pkt->SetVirtualSeq(seq++);
    pkt->SetPayload("Hello World!");
    log->Info("Transmitting packet {} to {}", pkt->GetVirtualSeq(), pkt->GetVirtualDestAddr());
    node << pkt;
    std::this_thread::sleep_for(chrono::seconds(1));
  }
}
