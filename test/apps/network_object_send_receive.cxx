#include "serialization/NetworkObjectReceiver.hpp"
#include "serialization/NetworkObjectSender.hpp"
// clang-format off
#include <string> // This is here to workaround moo issue #12
#include "serialization/fsd/Msgp.hpp"
#include "serialization/fsd/Nljs.hpp"
#include "serialization/fsd/Structs.hpp"
// clang-format on

#include <iostream>

int
main()
{
  using FakeData = dunedaq::serialization::fsd::FakeData;

  dunedaq::serialization::networkobjectsender::Conf sender_conf;
  sender_conf.ipm_plugin_type = "ZmqSender";
  sender_conf.stype = "json";
  sender_conf.address = "inproc://foo";

  dunedaq::serialization::networkobjectreceiver::Conf receiver_conf;
  receiver_conf.ipm_plugin_type = "ZmqReceiver";
  receiver_conf.address = "inproc://foo";

  dunedaq::serialization::NetworkObjectSender<FakeData> sender(sender_conf);
  dunedaq::serialization::NetworkObjectReceiver<FakeData> receiver(receiver_conf);

  FakeData fd;
  fd.fake_count = 25;

  sender.send(fd, std::chrono::milliseconds(2));
  FakeData fd_recv = receiver.recv(std::chrono::milliseconds(2));
  std::cout << "Sent: " << fd.fake_count << ". Received: " << fd_recv.fake_count << std::endl;
}
