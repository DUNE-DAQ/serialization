/**
 * @file non_moo_type.cxx
 *
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "logging/Logging.hpp"
#include "serialization/Serialization.hpp"

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/variadic/size.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

#include <string>
#include <vector>

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

} // end namespace myns

DUNE_DAQ_SERIALIZE_NON_INTRUSIVE(myns, MyTypeNonIntrusive, count, name, values)

template<class T>
bool
roundtrip(dunedaq::serialization::SerializationType& stype)
{
  T m;
  m.count = 3;
  m.name = "foo";
  m.values.push_back(3.1416);

  namespace ser = dunedaq::serialization;

  std::vector<uint8_t> bytes = ser::serialize(m, stype); // NOLINT(build/unsigned)
  T m_recv = ser::deserialize<T>(bytes);
  bool ok = true;
  if (m_recv.count != m.count) {
    TLOG(TLVL_ERROR) << "count does not match";
    ok = false;
  }
  if (m_recv.name != m.name) {
    TLOG(TLVL_ERROR) << "name does not match";
    ok = false;
  }
  if (m_recv.values != m.values) {
    TLOG(TLVL_ERROR) << "values does not match";
    ok = false;
  }
  return ok;
}

int
main()
{
  // Test all four combinations of { intrusive, non-intrusive } x { msgpack, json }
  bool ok = true;
  for (auto stype : { dunedaq::serialization::kMsgPack, dunedaq::serialization::kJSON }) {
    ok = ok && roundtrip<myns::MyTypeIntrusive>(stype);
    ok = ok && roundtrip<myns::MyTypeNonIntrusive>(stype);
  }
  if (!ok) {
    TLOG(TLVL_ERROR) << "Failure";
    exit(1);
  } else {
    TLOG() << "Success";
  }
  exit(0);
}
