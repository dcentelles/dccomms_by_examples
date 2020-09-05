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

  node->SetCommsDeviceId("bluerov1-dev");
  node->SetLogLevel(warn);
  node->Start();

  Ptr<Logger> log = CreateObject<Logger>();
  log->SetLogName("RX");
  log->FlushLogOn(info);

  auto pkt = pb->Create();
  char msg[100];
  while (true) {
    node >> pkt;
    if (pkt->IsOk()) {
      memcpy(msg, pkt->GetPayloadBuffer(), pkt->GetPayloadSize());
      msg[pkt->GetPayloadSize()] = 0;
      log->Info("Packet received from {}, Seq. {}: {}", pkt->GetSrc(), pkt->GetSeq(), msg);
    } else {
      log->Warn("Packet received with errors");
    }
  }
}
