/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains struct and other type definitions for shema in 
 * namespace dunedaq::serialization::fsd.
 */
#ifndef DUNEDAQ_SERIALIZATION_FSD_STRUCTS_HPP
#define DUNEDAQ_SERIALIZATION_FSD_STRUCTS_HPP

#include <cstdint>

#include <vector>

namespace dunedaq::serialization::fsd {

    // @brief A count of not too many things
    using Count = int32_t;


    // @brief A fake timestamp
    using Timestamp = int64_t;


    // @brief Fake Serializable data
    struct FakeData {

        // @brief A fake count of something
        Count fake_count = -4;
    };

    // @brief A sequence of FakeData
    using FakeDatas = std::vector<dunedaq::serialization::fsd::FakeData>;

    // @brief The amount of fakeness
    enum class Fakeness: unsigned {
        Unknown,
        Fake,
        SuperFake,
    };
    // return a string representation of a Fakeness.
    inline
    const char* str(Fakeness val) {
        if (val == Fakeness::Unknown) { return "Unknown" ;}
        if (val == Fakeness::Fake) { return "Fake" ;}
        if (val == Fakeness::SuperFake) { return "SuperFake" ;}
        return "";                  // should not reach
    }
    inline
    Fakeness parse_Fakeness(std::string val, Fakeness def = Fakeness::Unknown) {
        if (val == "Unknown") { return Fakeness::Unknown; }
        if (val == "Fake") { return Fakeness::Fake; }
        if (val == "SuperFake") { return Fakeness::SuperFake; }
        return def;
    }

    // @brief Another fake Serializable data
    struct AnotherFakeData {

        // @brief A fake count of something
        Count fake_count = -4;

        // @brief A fake timestamp for the data
        Timestamp fake_timestamp = 0;

        // @brief 
        FakeDatas fake_datas = {};

        // @brief 
        Fakeness fakeness = dunedaq::serialization::fsd::Fakeness::Unknown;
    };

} // namespace dunedaq::serialization::fsd

#endif // DUNEDAQ_SERIALIZATION_FSD_STRUCTS_HPP