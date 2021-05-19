/**
 * @file Serialization_test.cxx Serialization namespace Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "serialization/Serialization.hpp"
#include "serialization/serialize_variant.hpp"

/**
 * @brief Name of this test module
 */
#define BOOST_TEST_MODULE Serialization_test // NOLINT

#include "boost/test/data/test_case.hpp"
#include "boost/test/unit_test.hpp"

#include <string>
#include <thread>
#include <vector>

// A type that's made serializable "intrusively", ie, by changing the type itself
struct MyTypeIntrusive
{
  int count;
  std::string name;
  std::vector<double> values;

  DUNE_DAQ_SERIALIZE(MyTypeIntrusive, count, name, values);
};

namespace test {
// A type that's made serializable "intrusively", ie, by changing the type itself
struct MyTypeNonIntrusive
{
  float a_float;
  std::vector<int> values;
};

}

DUNE_DAQ_SERIALIZE_NON_INTRUSIVE(test, MyTypeNonIntrusive, a_float, values)

BOOST_AUTO_TEST_SUITE(Serialization_test)

/**
 * @brief Check that we can serialize -> deserialize and get back what we started with
 */
BOOST_DATA_TEST_CASE(SerializationRoundTrip,
                     boost::unit_test::data::make({ dunedaq::serialization::kMsgPack, dunedaq::serialization::kJSON }))
{

  MyTypeIntrusive m;
  m.count = 3;
  m.name = "foo";
  m.values.push_back(3.1416);
  m.values.push_back(2.781);

  namespace ser = dunedaq::serialization;

  std::vector<uint8_t> bytes = ser::serialize(m, sample);
  MyTypeIntrusive m_recv = ser::deserialize<MyTypeIntrusive>(bytes);
  BOOST_CHECK_EQUAL(m_recv.count, m.count);
  BOOST_CHECK_EQUAL(m_recv.name, m.name);
  BOOST_CHECK_EQUAL_COLLECTIONS(m_recv.values.begin(), m_recv.values.end(), m.values.begin(), m.values.end());
}

BOOST_DATA_TEST_CASE(SerializeVariant,
                     boost::unit_test::data::make({ dunedaq::serialization::kMsgPack, dunedaq::serialization::kJSON }))
{
  MyTypeIntrusive m;
  m.count = 3;
  m.name = "foo";
  m.values.push_back(3.1416);
  m.values.push_back(2.781);

  namespace ser = dunedaq::serialization;

  using VariantType = std::variant<MyTypeIntrusive, test::MyTypeNonIntrusive>;

  {
    VariantType v = m;
    std::vector<uint8_t> bytes = ser::serialize(v, sample);
    // std::cout << "Serialized bytes: ";
    // for(auto& byte : bytes) std::cout << byte << " ";
    // std::cout << std::endl;
    VariantType v_recv = ser::deserialize<VariantType>(bytes);
    BOOST_CHECK_EQUAL(v_recv.index(), v.index());
    MyTypeIntrusive mv_recv=std::get<MyTypeIntrusive>(v_recv);
    BOOST_CHECK_EQUAL(mv_recv.count, m.count);
    BOOST_CHECK_EQUAL(mv_recv.name, m.name);
    BOOST_CHECK_EQUAL_COLLECTIONS(mv_recv.values.begin(), mv_recv.values.end(), m.values.begin(), m.values.end());
  }

  {
    test::MyTypeNonIntrusive m2;
    m2.a_float = 1.0;
    m2.values = { 1, 2, 3 };
    
    VariantType v = m2;
    std::vector<uint8_t> bytes = ser::serialize(v, sample);
    // std::cout << "Serialized bytes: ";
    // for(auto& byte : bytes) std::cout << byte << " ";
    // std::cout << std::endl;
    VariantType v_recv = ser::deserialize<VariantType>(bytes);
    BOOST_CHECK_EQUAL(v_recv.index(), v.index());
    test::MyTypeNonIntrusive mv_recv=std::get<test::MyTypeNonIntrusive>(v_recv);
    BOOST_CHECK_EQUAL(mv_recv.a_float, m2.a_float);
    BOOST_CHECK_EQUAL_COLLECTIONS(mv_recv.values.begin(), mv_recv.values.end(), m2.values.begin(), m2.values.end());
  }
}

BOOST_AUTO_TEST_CASE(InvalidSerializationTypes)
{
  BOOST_CHECK_THROW(dunedaq::serialization::from_string("not a real type"),
                    dunedaq::serialization::UnknownSerializationTypeString);

  // The first byte, which indicates the message serialization type,
  // should be 'M' or 'J': check we get an exception when it's not
  std::vector<char> invalid_message = { '0', '2', '3', '4' };
  BOOST_CHECK_THROW(dunedaq::serialization::deserialize<int>(invalid_message),
                    dunedaq::serialization::UnknownSerializationTypeByte);

  std::vector<char> invalid_json_message = { 'J', ']', '[', '4' };
  BOOST_CHECK_THROW(dunedaq::serialization::deserialize<int>(invalid_json_message),
                    dunedaq::serialization::CannotDeserializeMessage);

  // An invalid msgpack message: we have our serialization type byte,
  // 'M', followed by 0xce, which indicates that a four-byte integer
  // follows. But we only have two more bytes after that, so the
  // message is invalid
  std::vector<unsigned char> invalid_msgpack_message = { 'M', 0xce, 0x0, 0x0 };
  BOOST_CHECK_THROW(dunedaq::serialization::deserialize<int>(invalid_json_message),
                    dunedaq::serialization::CannotDeserializeMessage);
}

BOOST_AUTO_TEST_SUITE_END()
