/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains functions struct and other type definitions for shema in 
 * namespace dunedaq::serialization::networkobjectreceiver to be serialized via nlohmann::json.
 */
#ifndef DUNEDAQ_SERIALIZATION_NETWORKOBJECTRECEIVER_NLJS_HPP
#define DUNEDAQ_SERIALIZATION_NETWORKOBJECTRECEIVER_NLJS_HPP

// My structs
#include "serialization/networkobjectreceiver/Structs.hpp"


#include <nlohmann/json.hpp>

namespace dunedaq::serialization::networkobjectreceiver {

    using data_t = nlohmann::json;
    
    inline void to_json(data_t& j, const Conf& obj) {
        j["stype"] = obj.stype;
        j["ipm_plugin_type"] = obj.ipm_plugin_type;
        j["address"] = obj.address;
    }
    
    inline void from_json(const data_t& j, Conf& obj) {
        if (j.contains("stype"))
            j.at("stype").get_to(obj.stype);    
        if (j.contains("ipm_plugin_type"))
            j.at("ipm_plugin_type").get_to(obj.ipm_plugin_type);    
        if (j.contains("address"))
            j.at("address").get_to(obj.address);    
    }
    
} // namespace dunedaq::serialization::networkobjectreceiver

#endif // DUNEDAQ_SERIALIZATION_NETWORKOBJECTRECEIVER_NLJS_HPP