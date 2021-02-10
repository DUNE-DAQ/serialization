/**
 * @file Serialization_test.cxx Serialization namespace Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "serialization/Serialization.hpp"
#include "serialization/NetworkObjectSender.hpp"
#include "serialization/NetworkObjectReceiver.hpp"


/**
 * @brief Name of this test module
 */
#define BOOST_TEST_MODULE Serialization_test // NOLINT

#include "boost/test/unit_test.hpp"
#include "boost/test/data/test_case.hpp"

#include <string>
#include <vector>
#include <thread>

// A type that's made serializable "intrusively", ie, by changing the type itself
struct MyTypeIntrusive
{
  int count;
  std::string name;
  std::vector<double> values;
  
  DUNE_DAQ_SERIALIZE(MyTypeIntrusive, count, name, values);
};

BOOST_AUTO_TEST_SUITE(Serialization_test)


/**
 * @brief Check that we can serialize -> deserialize and get back what we started with
 */
BOOST_DATA_TEST_CASE(SerializationRoundTrip, boost::unit_test::data::make({dunedaq::serialization::kMsgPack, dunedaq::serialization::kJSON}))
{

  MyTypeIntrusive m;
  m.count = 3;
  m.name = "foo";
  m.values.push_back(3.1416);
  m.values.push_back(2.781);

  namespace ser = dunedaq::serialization;

  std::vector<uint8_t> bytes = ser::serialize(m, sample);
  MyTypeIntrusive m_recv = ser::deserialize<MyTypeIntrusive>(bytes);
  BOOST_REQUIRE_EQUAL(m_recv.count,  m.count);
  BOOST_REQUIRE_EQUAL(m_recv.name,   m.name);
  BOOST_REQUIRE_EQUAL_COLLECTIONS(m_recv.values.begin(), m_recv.values.end(),
                                  m.values.begin(), m.values.end());
  
}

BOOST_DATA_TEST_CASE(NetworkObjectSenderReceiver, boost::unit_test::data::make({"json", "msgpack"}))
{
  // This function is run in a loop with the two serialization
  // types. Sometimes we get back to the top of the loop before the
  // inproc connection is closed, and we get an "address already in
  // use" error. Hack around that by just sleeping here
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  
  dunedaq::serialization::networkobjectsender::Conf sender_conf;
  sender_conf.ipm_plugin_type = "ZmqSender";
  sender_conf.stype = sample;
  sender_conf.address = "inproc://foo";

  dunedaq::serialization::networkobjectreceiver::Conf receiver_conf;
  receiver_conf.ipm_plugin_type = "ZmqReceiver";
  receiver_conf.address = "inproc://foo";

  dunedaq::NetworkObjectSender<MyTypeIntrusive> sender(sender_conf);
  dunedaq::NetworkObjectReceiver<MyTypeIntrusive> receiver(receiver_conf);

  MyTypeIntrusive m;
  m.count = 3;
  m.name = "foo";
  m.values.push_back(3.1416);
  m.values.push_back(2.781);

  sender.send(m, std::chrono::milliseconds(2));
  MyTypeIntrusive m_recv = receiver.recv(std::chrono::milliseconds(2));

  BOOST_REQUIRE_EQUAL(m_recv.count,  m.count);
  BOOST_REQUIRE_EQUAL(m_recv.name,   m.name);
  BOOST_REQUIRE_EQUAL_COLLECTIONS(m_recv.values.begin(), m_recv.values.end(),
                                  m.values.begin(),      m.values.end());

}

BOOST_AUTO_TEST_SUITE_END()
