/**
 * @file NetworkObjectReceiver.hpp
 *
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef SERIALIZATION_INCLUDE_SERIALIZATION_NETWORKOBJECTRECEIVER_HPP_
#define SERIALIZATION_INCLUDE_SERIALIZATION_NETWORKOBJECTRECEIVER_HPP_

#include "serialization/Serialization.hpp"
#include "serialization/networkobjectreceiver/Nljs.hpp"
#include "serialization/networkobjectreceiver/Structs.hpp"

#include "ipm/Receiver.hpp"

#include <memory> // for shared_ptr
namespace dunedaq::serialization {

/**
 * @brief NetworkObjectReceiver receives objects over IPM connections
 *
 * NetworkObjectReceiver and its counterpart NetworkObjectSender
 * provide a convenient interface to object serialization/sending
 * and receiving/deserialization over network connections. Any class
 * which can be converted to/from an @c nlohmann::json object can be
 * used; in particular, all classes generated with moo schema are
 * suitable for use with NetworkObjectSender/Receiver
 *
 * Typical usage:
 *
 * @code
 * NetworkObjectReceiver<MyClass> receiver(conf_object);
 * MyClass m = receiver.recv(m, std::chrono::milliseconds(200));
 * @endcode
 *
 */
template<class T>
class NetworkObjectReceiver
{
public:
  explicit NetworkObjectReceiver(const dunedaq::serialization::networkobjectreceiver::Conf& conf)
    : m_receiver(dunedaq::ipm::make_ipm_receiver(conf.ipm_plugin_type))
  {
    m_receiver->connect_for_receives({ { "connection_string", conf.address } });
  }

  T recv(const dunedaq::ipm::Receiver::duration_t& timeout)
  {
    dunedaq::ipm::Receiver::Response recvd = m_receiver->receive(timeout);
    return serialization::deserialize<T>(recvd.m_data);
  }

protected:
  std::shared_ptr<ipm::Receiver> m_receiver;
};
} // namespace dunedaq::serialization

#endif // SERIALIZATION_INCLUDE_SERIALIZATION_NETWORKOBJECTRECEIVER_HPP_
