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

#include "ipm/Sender.hpp"
#include "serialization/Serialization.hpp"

#include "serialization/networkobjectsender/Structs.hpp"
#include "serialization/networkobjectsender/Nljs.hpp"

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
    NetworkObjectSender(const dunedaq::serialization::networkobjectsender::Conf& conf)
      : sender_(dunedaq::ipm::makeIPMSender(conf.ipm_plugin_type))
      , stype_(dunedaq::serialization::fromString(conf.stype))
    {
      // TODO: We should get a moo.any object from the conf and just pass it straight through
      sender_->connect_for_sends({ {"connection_string", conf.address} });
    }

    /**
     * @brief Send object @p obj with timeout @p timeout
     */
    void send(const T& obj, const dunedaq::ipm::Sender::duration_type& timeout)
    {
      auto s=serialization::serialize(obj, stype_);
      sender_->send(s.data(), s.size(), timeout);
    }
    
  protected:
    std::shared_ptr<ipm::Sender> sender_;
    serialization::SerializationType stype_;
  };
}

#endif
