/**
 * @file NetworkObjectSender.hpp
 *
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef SERIALIZATION_INCLUDE_SERIALIZATION_NETWORKOBJECTSENDER_HPP_
#define SERIALIZATION_INCLUDE_SERIALIZATION_NETWORKOBJECTSENDER_HPP_

#include "serialization/Serialization.hpp"
#include "serialization/networkobjectsender/Nljs.hpp"
#include "serialization/networkobjectsender/Structs.hpp"

#include "ipm/Sender.hpp"

#include <memory> // for shared_ptr

namespace dunedaq {

/**
 * @brief NetworkObjectSender sends objects over IPM connections
 *
 * NetworkObjectSender and its counterpart NetworkObjectReceiver
 * provide a convenient interface to object serialization and
 * sending and receiving over network connections. Any class which
 * can be converted to/from an @c nlohmann::json object can be used;
 * in particular, all classes generated with moo schema are suitable
 * for use with NetworkObjectSender/Receiver
 *
 * Typical usage:
 *
 * @code
 * NetworkObjectSender<MyClass> sender(conf_object);
 * MyClass m;
 * // Set some fields of m...
 * sender.send(m, std::chrono::milliseconds(2));
 * @endcode
 *
 */
template<class T>
class NetworkObjectSender
{
public:
  explicit NetworkObjectSender(const dunedaq::serialization::networkobjectsender::Conf& conf)
    : m_sender(dunedaq::ipm::make_ipm_sender(conf.ipm_plugin_type))
    , m_stype(dunedaq::serialization::from_string(conf.stype))
  {
    m_sender->connect_for_sends({ { "connection_string", conf.address } });
  }

  /**
   * @brief Send object @p obj with timeout @p timeout
   */
  void send(const T& obj, const dunedaq::ipm::Sender::duration_t& timeout)
  {
    auto s = serialization::serialize(obj, m_stype);
    m_sender->send(s.data(), s.size(), timeout);
  }

protected:
  std::shared_ptr<ipm::Sender> m_sender;
  serialization::SerializationType m_stype;
};
} // namespace dunedaq

#endif // SERIALIZATION_INCLUDE_SERIALIZATION_NETWORKOBJECTSENDER_HPP_
