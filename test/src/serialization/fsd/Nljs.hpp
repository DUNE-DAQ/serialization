/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains functions struct and other type definitions for shema in 
 * namespace dunedaq::serialization::fsd to be serialized via nlohmann::json.
 */
#ifndef DUNEDAQ_SERIALIZATION_FSD_NLJS_HPP
#define DUNEDAQ_SERIALIZATION_FSD_NLJS_HPP

// My structs
#include "serialization/fsd/Structs.hpp"


#include <nlohmann/json.hpp>

namespace dunedaq::serialization::fsd {

    using data_t = nlohmann::json;    NLOHMANN_JSON_SERIALIZE_ENUM( Fakeness, {
            { dunedaq::serialization::fsd::Fakeness::Unknown, "Unknown" },
            { dunedaq::serialization::fsd::Fakeness::Fake, "Fake" },
            { dunedaq::serialization::fsd::Fakeness::SuperFake, "SuperFake" },
        })

    
    inline void to_json(data_t& j, const FakeData& obj) {
        j["fake_count"] = obj.fake_count;
    }
    
    inline void from_json(const data_t& j, FakeData& obj) {
        if (j.contains("fake_count"))
            j.at("fake_count").get_to(obj.fake_count);    
    }
    
    inline void to_json(data_t& j, const AnotherFakeData& obj) {
        j["fake_count"] = obj.fake_count;
        j["fake_timestamp"] = obj.fake_timestamp;
        j["fake_datas"] = obj.fake_datas;
        j["fakeness"] = obj.fakeness;
    }
    
    inline void from_json(const data_t& j, AnotherFakeData& obj) {
        if (j.contains("fake_count"))
            j.at("fake_count").get_to(obj.fake_count);    
        if (j.contains("fake_timestamp"))
            j.at("fake_timestamp").get_to(obj.fake_timestamp);    
        if (j.contains("fake_datas"))
            j.at("fake_datas").get_to(obj.fake_datas);    
        if (j.contains("fakeness"))
            j.at("fakeness").get_to(obj.fakeness);    
    }
    
} // namespace dunedaq::serialization::fsd

#endif // DUNEDAQ_SERIALIZATION_FSD_NLJS_HPP