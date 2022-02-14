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

#include "boost/preprocessor.hpp"
#include "msgpack.hpp"
#include "nlohmann/json.hpp"

#include <algorithm>
#include <string>
#include <vector>

#define DUNE_DAQ_SERIALIZABLE(Type) \
 template<> struct dunedaq::serialization::is_serializable<Type> { static constexpr bool value = true; }

/**
 * @brief Macro to make a class/struct serializable intrusively
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
// NOLINTNEXTLINE(build/define_used)
#define DUNE_DAQ_SERIALIZE(Type, ...)                                                                                  \
  MSGPACK_DEFINE(__VA_ARGS__)                                                                                          \
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Type, __VA_ARGS__)

// Helper macros for DUNE_DAQ_SERIALIZE_NON_INTRUSIVE()
// NOLINTNEXTLINE(build/define_used)
#define OPACK(r, data, elem) o.pack(m.elem);
// NOLINTNEXTLINE
#define OUNPACK(r, data, elem) m.elem = o.via.array.ptr[i++].as<decltype(m.elem)>();

/**
 * @brief Macro to make a class/struct serializable non-intrusively
 *
 * Call the macro outside your class declaration, from the global
 * namespace. The first argument is the namespace of your class, the
 * second is the class name, and the rest of the arguments list the
 * member variables. Example:
 *
 *      namespace ns {
 *      struct MyType
 *      {
 *        int i;
 *        std::string s;
 *        std::vector<double> v;
 *      }
 *      }
 *
 *      DUNE_DAQ_SERIALIZE_NON_INTRUSIVE(ns, MyType, i, s, v);
 *
 */
// NOLINTNEXTLINE
#define DUNE_DAQ_SERIALIZE_NON_INTRUSIVE(NS, Type, ...)                                                                \
  DUNE_DAQ_SERIALIZABLE(NS::Type);                                                                                     \
  namespace NS {                                                                                                       \
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Type, __VA_ARGS__)                                                                \
  }                                                                                                                    \
  namespace msgpack {                                                                                                  \
  MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)                                                                \
  {                                                                                                                    \
    namespace adaptor {                                                                                                \
    template<>                                                                                                         \
    struct pack<NS::Type>                                                                                              \
    {                                                                                                                  \
      template<typename Stream>                                                                                        \
      packer<Stream>& operator()(msgpack::packer<Stream>& o, NS::Type const& m) const                                  \
      {                                                                                                                \
        o.pack_array(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__));                                                             \
        BOOST_PP_SEQ_FOR_EACH(OPACK, , BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))                                          \
        return o;                                                                                                      \
      }                                                                                                                \
    };                                                                                                                 \
    template<>                                                                                                         \
    struct convert<NS::Type>                                                                                           \
    {                                                                                                                  \
      msgpack::object const& operator()(msgpack::object const& o, NS::Type& m) const                                   \
      {                                                                                                                \
        if (o.type != msgpack::type::ARRAY)                                                                            \
          throw msgpack::type_error();                                                                                 \
        if (o.via.array.size != BOOST_PP_VARIADIC_SIZE(__VA_ARGS__))                                                   \
          throw msgpack::type_error();                                                                                 \
        int i = 0;                                                                                                     \
        BOOST_PP_SEQ_FOR_EACH(OUNPACK, , BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))                                        \
        return o;                                                                                                      \
      }                                                                                                                \
    };                                                                                                                 \
    }                                                                                                                  \
  }                                                                                                                    \
  }

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
                  ((char)t))                            // attributes // NOLINT

ERS_DECLARE_ISSUE(serialization,                        // namespace
                  CannotDeserializeMessage,             // issue name
                  "Cannot deserialize message",)        // message

// clang-format on

namespace serialization {

    template<typename T> struct is_serializable : std::false_type {};

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

constexpr uint8_t // NOLINT(build/unsigned)
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

/**
 * @brief Serialize object @p obj using serialization method @p stype
 */
template<class T>
std::vector<uint8_t> // NOLINT(build/unsigned)
serialize(const T& obj, SerializationType stype)
{
  switch (stype) {
    case kJSON: {
      nlohmann::json j = obj;
      nlohmann::json::string_t s = j.dump();
      std::vector<uint8_t> ret(s.size() + 1); // NOLINT(build/unsigned)
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
      std::vector<uint8_t> ret(buf.size() + 1); // NOLINT(build/unsigned)
      ret[0] = serialization_type_byte(stype);
      std::copy(buf.data(), buf.data() + buf.size(), ret.begin() + 1); // NOLINT
      return ret;
    }
    default:
      throw UnknownSerializationTypeEnum(ERS_HERE);
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
      try {
        json j = json::parse(v.begin() + 1, v.end());
        return j.get<T>();
      } catch (json::exception& e) {
        throw CannotDeserializeMessage(ERS_HERE, e);
      }
    }
    case serialization_type_byte(kMsgPack): {
      try {
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
        msgpack::object_handle oh = msgpack::unpack(
          const_cast<char*>(reinterpret_cast<const char*>(v.data() + 1)),
          v.size() - 1,
          [](msgpack::type::object_type /*typ*/, std::size_t /*length*/, void* /*user_data*/) -> bool { return true; });
        msgpack::object obj = oh.get();
        return obj.as<T>();
      } catch (msgpack::type_error& e) {
        throw CannotDeserializeMessage(ERS_HERE, e);
      } catch (msgpack::unpack_error& e) {
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
