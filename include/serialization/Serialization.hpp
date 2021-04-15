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

#include "ers/Issue.hpp"

#include "msgpack.hpp"
#include "nlohmann/json.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <type_traits>

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

// clang-format off
ERS_DECLARE_ISSUE(serialization,                        // namespace
                  UnknownSerializationTypeString,       // issue name
                  "Unknown serialization type " << t,   // message
                  ((std::string)t))                     // attributes

ERS_DECLARE_ISSUE(serialization,                        // namespace
                  UnknownSerializationTypeEnum,         // issue name
                  "Unknown serialization type",)        // message

ERS_DECLARE_ISSUE(serialization,                        // namespace
                  UnknownSerializationTypeByte,         // issue name
                  "Unknown serialization type " << t,   // message
                  ((char)t))                            // attributes

ERS_DECLARE_ISSUE(serialization,                        // namespace
                  CannotDeserializeMessage,             // issue name
                  "Cannot deserialize message",)        // message



// clang-format on

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
  throw UnknownSerializationTypeString(ERS_HERE, s);
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
      throw UnknownSerializationTypeEnum(ERS_HERE);
  }
}

// This template specialization gets called if type T *can* be converted
// to nlohmann::json, which happens if the user has written the
// appropriate to_json function. (In that case, the user should have
// also written a corresponding from_json function, but the template
// matching doesn't check that.)
template<class T,
         std::enable_if_t<std::is_convertible<T, nlohmann::json>::value, bool> = true>
std::vector<uint8_t> // NOLINT
serialize_impl_json(const T& obj)
{
  nlohmann::json j = obj;
  nlohmann::json::string_t s = j.dump();
  std::vector<uint8_t> ret(s.size() + 1);
  ret[0] = serialization_type_byte(kJSON);
  std::copy(s.begin(), s.end(), ret.begin() + 1); // NOLINT
  return ret;
}

// This template specialization gets called if type T *cannot* be
// converted to nlohmann::json, but *can* be converted to msgpack. The
// object of type T is serialized to a msgpack buffer, which is then
// converted to json. This process is not efficient, but if you want
// performance, you're not using json anyway
//
// Typically, msgpack serializers put the class members into a msgpack
// array. JSON serializers (from to_json or DUNE_DAQ_SERIALIZE)
// typically put their members in a map, with keys equal to the member
// variable name. Since this function converts from the msgpack
// representation to json, we usually end up with the class
// represented as a json array.
//
// To spell it out, with:
//
//    class MyClass {
//      int x;
//      std::string y;
//    };
//
// `MyClass m{1, "foo"};` serialized via DUNE_DAQ_SERIALIZE would result in  
//
// { "x": 1, "y": "foo" }
//
// whereas this function would most likely serialize it as
//
// [ 1, "foo" ]
template<class T,
         std::enable_if_t<!std::is_convertible<T, nlohmann::json>::value, bool> = true>
std::vector<uint8_t> // NOLINT
serialize_impl_json(const T& obj)
{
  msgpack::sbuffer buf;
  msgpack::pack(buf, obj);
  std::vector<uint8_t> v(buf.data(), buf.data()+buf.size());
  nlohmann::json  j=nlohmann::json::from_msgpack(v);

  nlohmann::json::string_t s = j.dump();
  std::vector<uint8_t> ret(s.size() + 1);
  ret[0] = serialization_type_byte(kJSON);
  std::copy(s.begin(), s.end(), ret.begin() + 1); // NOLINT
  return ret;
}

template<class T>
std::vector<uint8_t> // NOLINT
serialize_impl_msgpack(const T& obj)
{
  // Serialize into the sbuffer and then copy to a
  // std::vector. Seems like it would be more efficient to
  // write directly to the vector (by creating a class that
  // implements `void write(char* buf, size_t len)`), but my
  // tests aren't any faster than this
  msgpack::sbuffer buf;
  msgpack::pack(buf, obj);
  std::vector<uint8_t> ret(buf.size() + 1);
  ret[0] = serialization_type_byte(kMsgPack);
  std::copy(buf.data(), buf.data() + buf.size(), ret.begin() + 1); // NOLINT
  return ret;
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
      return serialize_impl_json(obj);
    }
    case kMsgPack: {
      return serialize_impl_msgpack(obj);
    }
    default:
      throw UnknownSerializationTypeEnum(ERS_HERE);
  }
}


template<class T,
         std::enable_if_t<std::is_convertible<T, nlohmann::json>::value, bool> = true>
T
deserialize_impl_json(const nlohmann::json& j)
{
  using json = nlohmann::json;
  try{
    return j.get<T>();
  } catch(json::exception& e) {
    throw CannotDeserializeMessage(ERS_HERE, e);
  }
}

template<class T,
         std::enable_if_t<!std::is_convertible<T, nlohmann::json>::value, bool> = true>
T
deserialize_impl_json(const nlohmann::json& j)
{
  using json = nlohmann::json;
  try{
    std::vector<uint8_t> v = json::to_msgpack(j);
    msgpack::object_handle oh = msgpack::unpack((char*)(v.data()),
                                                v.size());
    msgpack::object obj = oh.get();
    return obj.as<T>();
  } catch(json::exception& e) {
    throw CannotDeserializeMessage(ERS_HERE, e);
  } catch(msgpack::type_error& e) {
    throw CannotDeserializeMessage(ERS_HERE, e);
  } catch(msgpack::unpack_error& e){
    throw CannotDeserializeMessage(ERS_HERE, e);
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
      try{
        json j = json::parse(v.begin() + 1, v.end());
        return deserialize_impl_json<T>(j);
      } catch(json::exception& e) {
        throw CannotDeserializeMessage(ERS_HERE, e);
      }
    }
    case serialization_type_byte(kMsgPack): {
      try{
        // The lambda function here is of type `unpack_reference_func`
        // as described at
        // https://github.com/msgpack/msgpack-c/wiki/v2_0_cpp_unpacker#memory-management
        // . It is called for every STR, BIN and EXT field in the
        // MsgPack data. If the function returns false, the object is
        // copied into MsgPack's "zone", otherwise a pointer to the
        // original buffer is stored. Our input buffer is going to exist
        // at least until the end of this function, so it's safe to
        // return true (ie, store a pointer in the MsgPack object; no
        // copy) everywhere. Doing so results in a factor ~2 speedup in
        // deserializing Fragment, which is just a large BIN field
        msgpack::object_handle oh = msgpack::unpack((char*)(v.data() + 1),
                                                    v.size() - 1,
                                                    [](msgpack::type::object_type /*typ*/, std::size_t /*length*/, void* /*user_data*/) -> bool {return true;}); // NOLINT
        msgpack::object obj = oh.get();
        return obj.as<T>();
      } catch(msgpack::type_error& e) {
        throw CannotDeserializeMessage(ERS_HERE, e);
      } catch(msgpack::unpack_error& e){
        throw CannotDeserializeMessage(ERS_HERE, e);
      }

    }
    default:
      throw UnknownSerializationTypeByte(ERS_HERE, (char)v[0]); // NOLINT
  }
}

} // namespace serialization
} // namespace dunedaq

#endif // SERIALIZATION_INCLUDE_SERIALIZATION_SERIALIZATION_HPP_
