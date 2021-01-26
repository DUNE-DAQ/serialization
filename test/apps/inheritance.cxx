#include "msgpack/adaptor/define_decl.hpp"
#include "serialization/Serialization.hpp"

#include <iostream>

// A type that's made serializable "intrusively", ie, by changing the type itself
struct Base
{
  int i;
  //  MSGPACK_DEFINE(i)
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Base, i);
};

struct Derived : public Base
{
  std::string s;
  // MSGPACK_DEFINE(s, MSGPACK_BASE(Base))
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Derived, s);
};

// These two functions provide the serialization/deserialization
// functionality for MsgPack
namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
{
  namespace adaptor {

  template<>
  struct pack<Base>
  {
    template<typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, Base const& b) const
    {
      // The number here is the number of members in the struct
      o.pack_array(1);
      o.pack(b.i);
      return o;
    }
  };

  template<>
  struct convert<Base>
  {
    msgpack::object const& operator()(msgpack::object const& o, Base& b) const
    {
      if (o.type != msgpack::type::ARRAY)
        throw msgpack::type_error();
      // The number here is the number of members in the struct
      if (o.via.array.size < 1)
        throw msgpack::type_error();
      b.i = o.via.array.ptr[0].as<int>();
      return o;
    }
  };

  template<>
  struct pack<Derived>
  {
    template<typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, Derived const& d) const
    {
      // The number here is the number of members in the struct
      o.pack_array(2);
      o.pack(d.i);
      o.pack(d.s);
      return o;
    }
  };

  template<>
  struct convert<Derived>
  {
    msgpack::object const& operator()(msgpack::object const& o, Derived& d) const
    {
      if (o.type != msgpack::type::ARRAY)
        throw msgpack::type_error();
      // The number here is the number of members in the struct
      if (o.via.array.size < 2)
        throw msgpack::type_error();
      d.i = o.via.array.ptr[0].as<int>();
      d.s = o.via.array.ptr[1].as<std::string>();
      return o;
    }
  };

  } // namespace adaptor
} // namespace MSGPACK_DEFAULT_API_NS
} // namespace msgpack

int
main()
{
  Derived d;
  d.i = 3;
  d.s = "foo";

  namespace ser = dunedaq::serialization;

  std::vector<uint8_t> bytes = ser::serialize(d, ser::MsgPack);
  std::cout << "Receiving to Derived" << std::endl;
  Derived d_recv = ser::deserialize<Derived>(bytes);
  assert(d_recv.i == d.i);
  assert(d_recv.s == d.s);
  std::cout << "Receiving to Base" << std::endl;
  Base b_recv = ser::deserialize<Base>(bytes);
  assert(b_recv.i == d.i);
}
