# DUNE DAQ C++ object serialization utilities

This repository contains utilities for serializing/deserializing C++ objects for DUNE DAQ

## Quick start

An appropriately-defined C++ type (see below) can be serialized/deserialized as follows:

```cpp
 MyClass m;
 m.some_member=3;
 // ... set other parts of m...
 dunedaq::serialization::SerializationType stype=dunedaq::serialization::MsgPack; // or JSON, which is human-readable but slower
 std::vector<uint8_t> bytes=dunedaq::serialization::serialize(m, stype);
 
 // ...elsewhere, after receiving the serialized object:
 MyClass m_recv=dunedaq::serialization::deserialize<MyClass>(bytes, stype);
```

If you want to send/receive the serialized object over [IPM](https://github.com/DUNE-DAQ/ipm), `NetworkObjectSender<T>` and `NetworkObjectReceiver<T>` provide a convenience wrapper:

```cpp
// Sender process:
NetworkObjectSender<FakeData> sender(sender_conf);
FakeData fd;
fd.fake_count=25;
sender.send(fd, std::chrono::milliseconds(2));
// Receiver process:
NetworkObjectReceiver<FakeData> receiver(receiver_conf);
FakeData fd_recv=receiver.recv(std::chrono::milliseconds(2));
// Now fd_recv.fake_count==25
```

See [network_object_send_receive.cxx](./test/apps/network_object_send_receive.cxx) for a full example, including setting the `sender_conf` and `receiver_conf` objects.

## Making types serializable

### With [`moo`](https://github.com/brettviren/moo)

If your type is specified via a `moo` schema, you just need to `moo render` your schema with the `onljs.hpp.j2` template (for json serialization) and with `omsgp.hpp.j2` (for MsgPack serialization; requires `moo` > 0.5.0). Then you will need to `#include` both of the generated headers wherever you serialize/deserialize objects of your type.

### Without [`moo`](https://github.com/brettviren/moo)

If your class is not specified via a `moo` schema, it can be made serializable by adding convertor functions for `nlohmann::json` and `msgpack`. (Right now, _both_ methods need to be implemented, even if you only plan to use one of them. Maybe this could be changed, but the serialization type can come from config, and not necessarily be known at compile-time, so code for both has to be available). Full instructions for serializing arbitrary types with `nlohmann::json` are available [here](https://nlohmann.github.io/json/features/arbitrary_types/) and for `msgpack`, [here](https://github.com/msgpack/msgpack-c/wiki/v2_0_cpp_packer).

The easiest way to achieve this is with the convenience macros provided by the two packages, eg:

```cpp
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
```

A complete example can be found in [`non_moo_type.cxx`](./test/apps/non_moo_type.cxx).
