/**
 * @file Serialization.hpp
 *
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef SERIALIZATION_INCLUDE_SERIALIZATION_SERIALIZATION_HPP_
#define SERIALIZATION_INCLUDE_SERIALIZATION_SERIALIZATION_HPP_

#include "ipm/Sender.hpp"

#include "msgpack.hpp"
#include "nlohmann/json.hpp"

#include <algorithm>
#include <string>
#include <vector>

/**
 * @brief Macro to make a class serializable
 *
 * Call the macro inside your class declaration, with the first
 * argument being the class name, followed by each of the member
 * variables. Example:
 * 
 *      struct MyType
 *      {
 *        int i;
 *        std::string s;
 *        std::vector<double> v;
 *
 *        DUNE_DAQ_SERIALIZE(MyType, i, s, v);
 *      };
 *
 */
#define DUNE_DAQ_SERIALIZE(Type, ...)                \
  MSGPACK_DEFINE(__VA_ARGS__)                        \
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Type, __VA_ARGS__)

namespace dunedaq {

namespace serialization {

/**
 * @brief Serialization methods that are available
 */
enum SerializationType
{
  kJSON,
  kMsgPack
};

/**
 * @brief Convert string to SerializationType
 */
inline SerializationType
from_string(const std::string s)
{
  if (s == "json")
    return kJSON;
  if (s == "msgpack")
    return kMsgPack;
  throw std::runtime_error("Unknown serialization type");
}

constexpr uint8_t
serialization_type_byte(SerializationType stype)
{
  switch (stype) {
    case kJSON:
      return 'J';
    case kMsgPack:
      return 'M';
    default:
      throw std::runtime_error("Unknown serialization type");
  }
}

/**
 * @brief Serialize object @p obj using serialization method @p stype
 */
template<class T>
std::vector<uint8_t> // NOLINT
serialize(const T& obj, SerializationType stype)
{
  switch (stype) {
    case kJSON: {
      nlohmann::json j = obj;
      nlohmann::json::string_t s = j.dump();
      std::vector<uint8_t> ret(s.size() + 1);
      ret[0] = serialization_type_byte(stype);
      std::copy(s.begin(), s.end(), ret.begin() + 1); // NOLINT
      return ret;
    }
    case kMsgPack: {
      // Serialize into the sbuffer and then copy to a
      // std::vector. Seems like it would be more efficient to
      // write directly to the vector (by creating a class that
      // implements `void write(char* buf, size_t len)`), but my
      // tests aren't any faster than this
      msgpack::sbuffer buf;
      msgpack::pack(buf, obj);
      std::vector<uint8_t> ret(buf.size() + 1);
      ret[0] = serialization_type_byte(stype);
      std::copy(buf.data(), buf.data() + buf.size(), ret.begin() + 1); // NOLINT
      return ret;
    }
    default:
      throw std::runtime_error("Unknown serialization type");
  }
}

/**
 * @brief Serialize object @p obj using serialization method @p stype, and send the resulting object using the IPM Sender @p sender with send timeout @p timeout
 *
 * In the (common) case where you are serializing an object in order to send it over an IPM connection, this function requires one less data copy than @a serialize. For large objects like Fragment, this can be worth a factor of 2 in speed
 */
template<class T>
void
serialize_and_send(const T& obj, SerializationType stype,
                   std::shared_ptr<dunedaq::ipm::Sender> sender, dunedaq::ipm::Sender::duration_t timeout)
{
  switch (stype) {
    case kJSON: {
      nlohmann::json j = obj;
      nlohmann::json::string_t s = j.dump();
      std::vector<uint8_t> vec(s.size()+1);
      vec[0]=serialization_type_byte(stype);
      std::copy(s.begin(), s.end(), vec.data() + 1); // NOLINT
      sender->send(vec.data(), vec.size(), timeout);
      return;
    }
    case kMsgPack: {
      msgpack::sbuffer buf;
      char tmp(serialization_type_byte(stype));
      buf.write(&tmp, 1);
      msgpack::pack(buf, obj);
      sender->send(buf.data(), buf.size(), timeout);
      return;
    }
    default:
      throw std::runtime_error("Unknown serialization type");
  }
}

/**
 * @brief Deserialize vector of bytes @p v into an instance of class @p T
 */
template<class T, typename CharType = unsigned char>
T
deserialize(const std::vector<CharType>& v)
{
  using json = nlohmann::json;

  // The first byte in the array indicates the serialization format;
  // the rest is the actual message
  switch (v[0]) {
    case serialization_type_byte(kJSON): {
      json j = json::parse(v.begin() + 1, v.end());
      return j.get<T>();
    }
    case serialization_type_byte(kMsgPack): {
      msgpack::object_handle oh = msgpack::unpack((char*)(v.data() + 1), v.size() - 1); // NOLINT
      msgpack::object obj = oh.get();
      return obj.as<T>();
    }
    default:
      throw std::runtime_error("Unknown serialization type");
  }
}

} // namespace serialization
} // namespace dunedaq

#endif // SERIALIZATION_INCLUDE_SERIALIZATION_SERIALIZATION_HPP_
