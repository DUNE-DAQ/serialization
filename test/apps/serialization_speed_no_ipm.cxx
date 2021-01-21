#include <string> // This is here to workaround moo issue #12
#include "serialization/fsd/Structs.hpp"
#include "serialization/fsd/Nljs.hpp"
#include "serialization/fsd/Msgp.hpp"
#include "serialization/Serialization.hpp"

#include <iostream>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

using AnotherFakeData=dunedaq::serialization::fsd::AnotherFakeData;
using FakeData=dunedaq::serialization::fsd::FakeData;

// Return the current steady clock in microseconds
inline uint64_t now_us() {
    using namespace std::chrono;
    // std::chrono is the worst
    return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
}

void time_serialization(dunedaq::serialization::SerializationType stype)
{
  const int N=1000000;
  int total=0;
  uint64_t start_time=now_us();
  AnotherFakeData fd;
  for(int i=0; i<20; ++i){
    fd.fake_datas.push_back(FakeData{3});
  }
  for(int i=0; i<N; ++i){
    fd.fake_count=i;
    fd.fakeness=dunedaq::serialization::fsd::Fakeness::SuperFake;
    std::vector<uint8_t> bytes=dunedaq::serialization::serialize(fd, stype);
    AnotherFakeData fd_recv=dunedaq::serialization::deserialize<AnotherFakeData>(bytes, stype);
    total+=fd_recv.fake_count;
  }
  printf("total: %d\n", total);
  uint64_t end_time=now_us();
  double time_taken_s=1e-6*(end_time - start_time);
  double kHz=1e-3*N/time_taken_s;
  printf("Sent %d messages in %.3fs (%.1f kHz)\n",
         N, time_taken_s, kHz);
}

int main()
{
  printf("MsgPack:\n");
  time_serialization(dunedaq::serialization::MsgPack);
  printf("JSON:\n");
  time_serialization(dunedaq::serialization::JSON);
}
