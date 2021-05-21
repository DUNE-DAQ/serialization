/**
 * @file serialization_speed_no_ipm.cxx
 *
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "logging/Logging.hpp"
#include "serialization/Serialization.hpp"
#include "serialization/fsd/MsgP.hpp"
#include "serialization/fsd/Nljs.hpp"
#include "serialization/fsd/Structs.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

using AnotherFakeData = dunedaq::serialization::fsd::AnotherFakeData;
using FakeData = dunedaq::serialization::fsd::FakeData;

// Return the current steady clock in microseconds
inline uint64_t // NOLINT(build/unsigned)
now_us()
{
  using namespace std::chrono;
  // std::chrono is the worst
  return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
}

void
time_serialization(dunedaq::serialization::SerializationType stype)
{
  const int N = 1000000;
  int total = 0;
  uint64_t start_time = now_us(); // NOLINT(build/unsigned)
  AnotherFakeData fd;
  for (int i = 0; i < 20; ++i) {
    fd.fake_datas.push_back(FakeData{ 3 });
  }
  for (int i = 0; i < N; ++i) {
    fd.fake_count = i;
    fd.fakeness = dunedaq::serialization::fsd::Fakeness::SuperFake;
    std::vector<uint8_t> bytes = dunedaq::serialization::serialize(fd, stype); // NOLINT(build/unsigned)
    AnotherFakeData fd_recv = dunedaq::serialization::deserialize<AnotherFakeData>(bytes);
    total += fd_recv.fake_count;
  }
  TLOG() << "total: " << total;
  uint64_t end_time = now_us(); // NOLINT(build/unsigned)
  double time_taken_s = 1e-6 * (end_time - start_time);
  double kHz = 1e-3 * N / time_taken_s;
  TLOG() << "Sent " << N << " messages in " << time_taken_s << " (" << kHz << " kHz)";
}

int
main()
{
  TLOG() << "MsgPack:";
  time_serialization(dunedaq::serialization::kMsgPack);
  TLOG() << "JSON:";
  time_serialization(dunedaq::serialization::kJSON);
}
