#include <cpplogging/cpplogging.h>
#include <cpputils/SignalManager.h>
#include <cxxopts.hpp>
#include <dccomms_packets/VariableLength2BPacket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace dccomms;
using namespace std;
using namespace cpputils;
using namespace dccomms_packets;

int main(int argc, const char **argv) {
  std::string logFile, logLevelStr = "info", nodeName;
  uint32_t dataRate = 999999, payloadSize = 20, tx_mac = 1, numPackets = 99999;
  bool flush = false, syncLog = false;
  try {
    // clang-format off
    cxxopts::Options options("tutorials",
                             " - command line options");
    options.add_options()
        ("f,log-file", "File to save the log", cxxopts::value<std::string>(logFile)->default_value(""))
        ("F,flush-log", "flush log", cxxopts::value<bool>(flush))
        ("s,sync-log", "sync-log", cxxopts::value<bool>(syncLog))
        ("l,log-level", "log level: critical,debug,err,info,off,trace,warn", cxxopts::value<std::string>(logLevelStr)->default_value("info"))
        ("help", "Print help");
    options.add_options("node_comms")
        ("dst-mac", "Destination MAC address", cxxopts::value<uint32_t>(tx_mac))
        ("payload-size", "payload size in bytes", cxxopts::value<uint32_t>(payloadSize))
        ("num-packets", "Number of transmitted packets", cxxopts::value<uint32_t>(numPackets))
        ("data-rate", "application data rate in bps (a high value could saturate the output buffer", cxxopts::value<uint32_t>(dataRate))
        ("node-name", "dccomms id", cxxopts::value<std::string>(nodeName)->default_value("bluerov0-dev"));

    // clang-format on
    auto result = options.parse(argc, argv);
    if (result.count("help")) {
      std::cout << options.help({"", "node_comms"}) << std::endl;
      exit(0);
    }

  } catch (const cxxopts::OptionException &e) {
    std::cout << "error parsing options: " << e.what() << std::endl;
    exit(1);
  }

  auto logFormatter =
      std::make_shared<spdlog::pattern_formatter>("%D %T.%F %v");
  LogLevel logLevel = cpplogging::GetLevelFromString(logLevelStr);
  Ptr<Logger> log = CreateObject<Logger>();
  if (logFile != "") {
    log->LogToFile(logFile);
  }
  log->SetLogLevel(logLevel);
  log->SetLogName(nodeName);
  log->LogToConsole(true);
  log->SetLogFormatter(logFormatter);

  if (flush) {
    log->FlushLogOn(info);
    log->Info("Flush log on info");
  }
  if (!syncLog) {
    log->SetAsyncMode();
    log->Info("Async. log");
  }

  auto pb = CreateObject<VariableLength2BPacketBuilder>();
  auto node = CreateObject<CommsDeviceService>(pb);

  log->Info("Node name: {}", nodeName);
  log->Info("Number of packets: {}", numPackets);
  log->Info("Data rate: {} bps", dataRate);
  log->Info("Payload size: {} B", payloadSize);
  log->Info("Destination: {}", tx_mac);
  node->SetCommsDeviceId(nodeName);
  node->SetLogLevel(warn);
  node->Start();

  std::thread tx, rx;

  tx = std::thread([&]() {
    auto pkt = pb->Create();
    uint32_t seq = 0;
    uint8_t *data = new uint8_t[payloadSize];
    memset(data, '#', payloadSize);
    double bytesPerSecond = dataRate / 8.;
    double nanosPerByte = 1e9 / bytesPerSecond;
    uint64_t nanos = static_cast<uint64_t>(round(payloadSize * nanosPerByte));
    uint32_t cont = 0;
    while (cont < numPackets) {
      pkt->SetVirtualDestAddr(tx_mac);
      pkt->SetVirtualSeq(seq++);
      pkt->SetPayload(data, payloadSize);
      pkt->PayloadUpdated(payloadSize);
      log->Info(
          "Transmitting packet {} to {}. Total size: {}. Payload size: {}.",
          pkt->GetVirtualSeq(), pkt->GetVirtualDestAddr(), pkt->GetPacketSize(),
          pkt->GetPayloadSize());
      node << pkt;
      std::this_thread::sleep_for(chrono::nanoseconds(nanos));
    }
  });

  rx = std::thread([&]() {
    auto pkt = pb->Create();
    while (true) {
      node >> pkt;
      if (pkt->IsOk()) {
        log->Info("Packet received from {}, Seq. {}. Total size: {}. "
                  "Payload size: {}",
                  pkt->GetSrc(), pkt->GetSeq(), pkt->GetPacketSize(),
                  pkt->GetPayloadSize());
      } else {
        log->Warn("Packet received with errors");
      }
    }
  });

  SignalManager::SetLastCallback(SIGINT, [&](int sig) {
    printf("Received %d signal.\nFlushing log messages...", sig);
    fflush(stdout);
    log->FlushLog();
    Utils::Sleep(2000);
    printf("Log messages flushed.\n");
    exit(0);
  });

  tx.join();
  rx.join();
}
