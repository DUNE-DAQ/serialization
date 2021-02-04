/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains struct and other type definitions for shema in 
 * namespace dunedaq::serialization::networkobjectreceiver.
 */
#ifndef DUNEDAQ_SERIALIZATION_NETWORKOBJECTRECEIVER_STRUCTS_HPP
#define DUNEDAQ_SERIALIZATION_NETWORKOBJECTRECEIVER_STRUCTS_HPP

#include <cstdint>

#include <string>

namespace dunedaq::serialization::networkobjectreceiver {

    // @brief Address to receive from
    using Address = std::string;

    // @brief IPM plugin type
    using IPMPluginType = std::string;

    // @brief NetworkObjectReceiver Configuration
    struct Conf {

        // @brief IPM plugin type
        IPMPluginType ipm_plugin_type = "ZmqSender";

        // @brief Address to receive from
        Address address = "inproc://default";
    };

} // namespace dunedaq::serialization::networkobjectreceiver

#endif // DUNEDAQ_SERIALIZATION_NETWORKOBJECTRECEIVER_STRUCTS_HPP