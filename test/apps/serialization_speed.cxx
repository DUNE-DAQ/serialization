#include "serialization/NetworkObjectReceiver.hpp"
#include "serialization/NetworkObjectSender.hpp"
// clang-format off
#include <string> // This is here to workaround moo issue #12
#include "serialization/fsd/Msgp.hpp"
#include "serialization/fsd/Nljs.hpp"
#include "serialization/fsd/Structs.hpp"

// clang-format on

#include <chrono>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

using FakeData = dunedaq::serialization::fsd::FakeData;

void
sender_thread_fn(dunedaq::serialization::networkobjectsender::Conf sender_conf, int n_messages)
{
  dunedaq::NetworkObjectSender<FakeData> sender(sender_conf);

  for (int i = 0; i < n_messages; ++i) {
    FakeData fd;
    fd.fake_count = 25;

    sender.send(fd, std::chrono::milliseconds(1000000));
  }
}

void
receiver_thread_fn(dunedaq::serialization::networkobjectreceiver::Conf receiver_conf, int n_messages)
{
  int total = 0;
  dunedaq::NetworkObjectReceiver<FakeData> receiver(receiver_conf);
  for (int i = 0; i < n_messages; ++i) {
    FakeData fd_recv = receiver.recv(std::chrono::milliseconds(1000000));
    total += fd_recv.fake_count;
  }
  std::cout << "Total:" << total << std::endl;
}

// Return the current steady clock in microseconds
inline uint64_t
now_us()
{
  using namespace std::chrono;
  // std::chrono is the worst
  return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
}

int
main()
{
  dunedaq::serialization::networkobjectsender::Conf sender_conf;
  sender_conf.ipm_plugin_type = "ZmqSender";
  sender_conf.stype = "msgpack";
  sender_conf.address = "inproc://foo";

  dunedaq::serialization::networkobjectreceiver::Conf receiver_conf;
  receiver_conf.ipm_plugin_type = "ZmqReceiver";
  receiver_conf.address = "inproc://foo";

  const int N = 1000000;
  uint64_t start_time = now_us();
  std::thread sender_thread(sender_thread_fn, sender_conf, N);
  std::thread receiver_thread(receiver_thread_fn, receiver_conf, N);
  sender_thread.join();
  receiver_thread.join();
  uint64_t end_time = now_us();
  double time_taken_s = 1e-6 * (end_time - start_time);
  double kHz = 1e-3 * N / time_taken_s;
  printf("Sent %d messages in %.3fs (%.1f kHz)\n", N, time_taken_s, kHz);
}
