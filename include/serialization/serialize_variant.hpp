/**
 * @file serialize_variant.hpp
 *
 * Helpers to allow serialization/deserialization of std::variant
 * types with msgpack and nlohmann::json. If you need to
 * serialize/deserialize a std::variant object in your code, you
 * should juse need to #include this file
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef SERIALIZATION_INCLUDE_SERIALIZATION_SERIALIZE_VARIANT_HPP_
#define SERIALIZATION_INCLUDE_SERIALIZATION_SERIALIZE_VARIANT_HPP_

#include "msgpack.hpp"
#include "nlohmann/json.hpp"

#include <variant>
#include <iostream>

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
namespace adaptor {

template<typename... Args>
struct pack<std::variant<Args...>> {
    template <typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, std::variant<Args...> const& v) const {
      // There are always exactly 2 items in the msgpack array: the
      // index indicating which type the variant is holding, and the
      // instance of the type itself (it *doesn't* depend sizeof...(Args))
      o.pack_array(2);
      o.pack(v.index());
      std::visit([&o](auto&& arg){o.pack(arg);}, v);
      return o;
    }
};

template<typename...Args>
struct convert<std::variant<Args...>> {

  // Base case for the variadic template function below. Should never be
  // called, but the compiler needs to see it
  template<typename VariantType>
  void set_variant_helper(std::size_t,  msgpack::object const&, VariantType&&) const
  {
  }

  // Deserializing std::variant is tricky, because we only know the
  // index of the type that's held at runtime. We want something that is effectively:
  //
  // std::variant<T0, T1, T2, ...> v;
  // size_t index = deserialize index...;
  // switch(index){
  // case 0:
  //   v = o.via.array.ptr.as<T0>(); break
  // case 1:
  //   v = o.via.array.ptr.as<T1>(); break
  // ...etc...
  //
  // This recursive variadic template function achieves that, using
  // the index as a regular function parameter (not a template
  // parameter, since it's only known at runtime). We peel off the
  // variant types one at a time, decreasing the index each time. When
  // the index reaches zero, we've peeled off enough types to get the
  // one we want, so we can set the std::variant
  template<typename VariantType, typename T, typename...Types>
  void set_variant_helper(std::size_t i, msgpack::object const& o, VariantType&& v) const
  {
    if(i==0) v=o.via.array.ptr[1].as<T>();
    else set_variant_helper<VariantType, Types...>(i-1, o, v);
  }
  
  msgpack::object const& operator()(msgpack::object const& o, std::variant<Args...>& v) const {
        if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
        // There are always exactly 2 items in the msgpack array: the
        // index indicating which type the variant is holding, and the
        // instance of the type itself (it *doesn't* depend sizeof...(Args))
        if (o.via.array.size != 2) throw msgpack::type_error();
        std::size_t index=o.via.array.ptr[0].as<std::size_t>();
        if(index>=sizeof...(Args)){
          throw msgpack::type_error();
        }
        set_variant_helper<std::variant<Args...>&, Args...>(index, o, v);
        return o;
    }
};


} // namespace adaptor
} // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
} // namespace msgpack


// nlohmann::json version adapted from
// https://github.com/nlohmann/json/issues/1261#issuecomment-426209912
namespace nlohmann
{
template <typename ...Args>
struct adl_serializer<std::variant<Args...>>
{
  // Base case for the variadic template function below. Should never be
  // called, but the compiler needs to see it
  template<typename VariantType>
  static void set_variant_helper(std::size_t, nlohmann::json const&, VariantType&&)
  {
  }

  template<typename VariantType, typename T, typename...Types>
  static void set_variant_helper(std::size_t i, nlohmann::json const& j, VariantType&& v)
  {
    if(i==0) v=j.get<T>();
    else set_variant_helper<VariantType, Types...>(i-1, j, v);
  }

  static void to_json(json& j, std::variant<Args...> const& v)
  {
    std::visit([&](auto&& value) {
      j["index"] = v.index();
      j["value"] = std::forward<decltype(value)>(value);
    }, v);
  }

  static void from_json(json const& j, std::variant<Args...>& v)
  {
    auto const index = j.at("index").get<int>();
    set_variant_helper<std::variant<Args...>&, Args...>(index, j.at("value"), v);
  }
};
}

#endif // SERIALIZATION_INCLUDE_SERIALIZATION_SERIALIZE_VARIANT_HPP_
