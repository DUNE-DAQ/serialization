/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains struct and other type definitions for shema in 
 * namespace dunedaq::serialization::networkobjectsender.
 */
#ifndef DUNEDAQ_SERIALIZATION_NETWORKOBJECTSENDER_STRUCTS_HPP
#define DUNEDAQ_SERIALIZATION_NETWORKOBJECTSENDER_STRUCTS_HPP

#include <cstdint>

#include <string>

namespace dunedaq::serialization::networkobjectsender {

    // @brief Address to send to
    using Address = std::string;

    // @brief String describing serialization type
    using SerializationString = std::string;

    // @brief IPM plugin type
    using IPMPluginType = std::string;

    // @brief NetworkObjectSender Configuration
    struct Conf {

        // @brief Serialization type
        SerializationString stype = "json";

        // @brief IPM plugin type
        IPMPluginType ipm_plugin_type = "ZmqSender";

        // @brief Address to send to
        Address address = "inproc://default";
    };

} // namespace dunedaq::serialization::networkobjectsender

#endif // DUNEDAQ_SERIALIZATION_NETWORKOBJECTSENDER_STRUCTS_HPP