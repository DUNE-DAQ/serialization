#include "serialization/Serialization.hpp"

namespace myns {

// A type that's made serializable "intrusively", ie, by changing the type itself
struct MyTypeIntrusive
{
  int i;
  std::string s;
  std::vector<double> v;

  // These are the macros that make the type serializable by MsgPack
  // and nlohmann::json respectively. Both are needed in order to use
  // the functions in `dunedaq::serialization`. There are some quirks:
  //
  // * MSGPACK_DEFINE should _not_ be followed by a `;`, in order to
  //   avoid a warning
  //
  // * The NLOHMANN macro requires the class name as its first
  //   argument, while MSGPACK_DEFINE does not
  MSGPACK_DEFINE(i, s, v)
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(MyTypeIntrusive, i, s, v);
};

// A type that's made serializable non-intrusively, ie without
// changing the class itself. You might do this if you need to
// serialize a third-party class that you can't change; if you
// prefer to keep your class free of extraneous functions; or if you
// are just masochistic
struct MyTypeNonIntrusive
{
  int i;
  std::string s;
  std::vector<double> v;
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
  j["i"] = m.i;
  j["s"] = m.s;
  j["v"] = m.v;
}

void
from_json(const nlohmann::json& j, MyTypeNonIntrusive& m)
{
  j.at("i").get_to(m.i);
  j.at("s").get_to(m.s);
  j.at("v").get_to(m.v);
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
      o.pack(m.i);
      o.pack(m.s);
      o.pack(m.v);
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
      m.i = o.via.array.ptr[0].as<int>();
      m.s = o.via.array.ptr[1].as<std::string>();
      m.v = o.via.array.ptr[2].as<std::vector<double>>();
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
  m.i = 3;
  m.s = "foo";
  m.v.push_back(3.1416);

  namespace ser = dunedaq::serialization;

  std::vector<uint8_t> bytes = ser::serialize(m, stype);
  T m_recv = ser::deserialize<T>(bytes);
  assert(m_recv.i == m.i);
  assert(m_recv.s == m.s);
  assert(m_recv.v == m.v);
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
