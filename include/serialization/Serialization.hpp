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

#include <msgpack.hpp>
#include <nlohmann/json.hpp>

#include <string>

namespace dunedaq {

namespace serialization {

/**
 * @brief Serialization methods that are available
 */
enum SerializationType
{
  JSON,
  MsgPack
};

/**
 * @brief Convert string to SerializationType
 */
SerializationType
fromString(const std::string s)
{
  if (s == "json")
    return JSON;
  if (s == "msgpack")
    return MsgPack;
  throw std::runtime_error("Unknown serialization type");
}

/**
 * @brief Serialize object @p obj using serialization method @p stype
 */
template<class T>
std::vector<uint8_t>
serialize(const T& obj, SerializationType stype)
{
  switch (stype) {
    case JSON: {
      nlohmann::json j = obj;
      nlohmann::json::string_t s = j.dump();
      std::vector<uint8_t> ret(s.begin(), s.end());
      return ret;
    }
    case MsgPack: {
      // Serialize into the sbuffer and then copy to a
      // std::vector. Seems like it would be more efficient to
      // write directly to the vector (by creating a class that
      // implements `void write(char* buf, size_t len)`), but my
      // tests aren't any faster than this
      msgpack::sbuffer buf;
      msgpack::pack(buf, obj);
      std::vector<uint8_t> v(buf.data(), buf.data() + buf.size());
      return v;
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
deserialize(const std::vector<CharType>& v, SerializationType stype)
{
  using json = nlohmann::json;

  switch (stype) {
    case JSON: {
      json j = json::parse(v);
      return j.get<T>();
    }
    case MsgPack: {
      msgpack::object_handle oh = msgpack::unpack((char*)v.data(), v.size());
      msgpack::object obj = oh.get();
      return obj.as<T>();
    }
    default:
      throw std::runtime_error("Unknown serialization type");
  }
}

} // namespace serialization

} // namespace dunedaq
#endif
