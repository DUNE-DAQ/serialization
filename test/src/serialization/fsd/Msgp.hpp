/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains functions struct and other type definitions for schema in
 * namespace dunedaq::serialization::fsd to be serialized via MsgPack.
 */
#ifndef DUNEDAQ_SERIALIZATION_FSD_MSGPACK_HPP
#define DUNEDAQ_SERIALIZATION_FSD_MSGPACK_HPP

// My structs
#include "serialization/fsd/Structs.hpp"

#include <msgpack.hpp>

MSGPACK_ADD_ENUM(dunedaq::serialization::fsd::Fakeness)

// MsgPack serialization/deserialization functions
namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
{
  namespace adaptor {

  // MsgPack serialization for RECORD type:
  // dunedaq::serialization::fsd::FakeData
  template<>
  struct convert<dunedaq::serialization::fsd::FakeData>
  {
    msgpack::object const& operator()(msgpack::object const& o, dunedaq::serialization::fsd::FakeData& v) const
    {
      if (o.type != msgpack::type::ARRAY)
        throw msgpack::type_error();
      if (o.via.array.size != 1)
        throw msgpack::type_error();
      v.fake_count = o.via.array.ptr[0].as<dunedaq::serialization::fsd::Count>();
      return o;
    }
  };
  template<>
  struct pack<dunedaq::serialization::fsd::FakeData>
  {
    template<typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, dunedaq::serialization::fsd::FakeData const& v) const
    {
      // packing member variables as an array.
      o.pack_array(1);
      o.pack(v.fake_count);
      return o;
    }
  };

  // MsgPack serialization for RECORD type:
  // dunedaq::serialization::fsd::AnotherFakeData
  template<>
  struct convert<dunedaq::serialization::fsd::AnotherFakeData>
  {
    msgpack::object const& operator()(msgpack::object const& o, dunedaq::serialization::fsd::AnotherFakeData& v) const
    {
      if (o.type != msgpack::type::ARRAY)
        throw msgpack::type_error();
      if (o.via.array.size != 4)
        throw msgpack::type_error();
      v.fake_count = o.via.array.ptr[0].as<dunedaq::serialization::fsd::Count>();
      v.fake_timestamp = o.via.array.ptr[1].as<dunedaq::serialization::fsd::Timestamp>();
      v.fake_datas = o.via.array.ptr[2].as<dunedaq::serialization::fsd::FakeDatas>();
      v.fakeness = o.via.array.ptr[3].as<dunedaq::serialization::fsd::Fakeness>();
      return o;
    }
  };
  template<>
  struct pack<dunedaq::serialization::fsd::AnotherFakeData>
  {
    template<typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, dunedaq::serialization::fsd::AnotherFakeData const& v) const
    {
      // packing member variables as an array.
      o.pack_array(4);
      o.pack(v.fake_count);
      o.pack(v.fake_timestamp);
      o.pack(v.fake_datas);
      o.pack(v.fakeness);
      return o;
    }
  };

  } // namespace adaptor
} // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
} // namespace msgpack

#endif // DUNEDAQ_SERIALIZATION_FSD_MSGPACK_HPP