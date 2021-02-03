#include "serialization/Serialization.hpp"

namespace myns {

// A type that's made serializable "intrusively", ie, by changing the type itself
struct MyTypeIntrusive
{
  int count;
  std::string name;
  std::vector<double> values;

  DUNE_DAQ_SERIALIZE(MyTypeIntrusive, count, name, values);
};

// A type that's made serializable non-intrusively, ie without
// changing the class itself. You might do this if you need to
// serialize a third-party class that you can't change; if you
// prefer to keep your class free of extraneous functions; or if you
// are just masochistic
struct MyTypeNonIntrusive
{
  int count;
  std::string name;
  std::vector<double> values;
};

// These two functions provide the serialization/deserialization
// functionality for nlohmann::json. They don't need to be in the same file as the
// definition of the type, but they do need to be in the same
// namespace. I assume that it's not strictly required to name the map
// keys the same thing as the variable, but it seems sensible
//
// For full instructions, see:
// https://nlohmann.github.io/json/features/arbitrary_types/
void
to_json(nlohmann::json& j, const MyTypeNonIntrusive& m)
{
  j["count"]  = m.count;
  j["name"]   = m.name;
  j["values"] = m.values;
}

void
from_json(const nlohmann::json& j, MyTypeNonIntrusive& m)
{
  j.at("count").get_to(m.count);
  j.at("name").get_to(m.name);
  j.at("values").get_to(m.values);
}

} // end namespace myns

// These two functions provide the serialization/deserialization
// functionality for MsgPack
namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
{
  namespace adaptor {

  template<>
  struct pack<myns::MyTypeNonIntrusive>
  {
    template<typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, myns::MyTypeNonIntrusive const& m) const
    {
      // The number here is the number of members in the struct
      o.pack_array(3);
      o.pack(m.count);
      o.pack(m.name);
      o.pack(m.values);
      return o;
    }
  };

  template<>
  struct convert<myns::MyTypeNonIntrusive>
  {
    msgpack::object const& operator()(msgpack::object const& o, myns::MyTypeNonIntrusive& m) const
    {
      if (o.type != msgpack::type::ARRAY)
        throw msgpack::type_error();
      // The number here is the number of members in the struct
      if (o.via.array.size != 3)
        throw msgpack::type_error();
      m.count = o.via.array.ptr[0].as<int>();
      m.name = o.via.array.ptr[1].as<std::string>();
      m.values = o.via.array.ptr[2].as<std::vector<double>>();
      return o;
    }
  };

  } // namespace adaptor
} // namespace MSGPACK_DEFAULT_API_NS
} // namespace msgpack

template<class T>
void
roundtrip(dunedaq::serialization::SerializationType& stype)
{
  T m;
  m.count = 3;
  m.name = "foo";
  m.values.push_back(3.1416);

  namespace ser = dunedaq::serialization;

  std::vector<uint8_t> bytes = ser::serialize(m, stype);
  T m_recv = ser::deserialize<T>(bytes);
  assert(m_recv.count  == m.count);
  assert(m_recv.name   == m.name);
  assert(m_recv.values == m.values);
}

int
main()
{
  // Test all four combinations of { intrusive, non-intrusive } x { msgpack, json }
  for (auto stype : { dunedaq::serialization::kMsgPack, dunedaq::serialization::kJSON }) {
    roundtrip<myns::MyTypeIntrusive>(stype);
    roundtrip<myns::MyTypeNonIntrusive>(stype);
  }
}
