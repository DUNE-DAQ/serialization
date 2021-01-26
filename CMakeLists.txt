cmake_minimum_required(VERSION 3.12)
project(serialization VERSION 0.0.0)

find_package(daq-cmake REQUIRED)
daq_setup_environment()

find_package(appfwk REQUIRED)
find_package(ipm REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(msgpack REQUIRED)

##############################################################################
# Main library

# We don't have a real library, but we want to create a target for
# dependents to be able to depend on
daq_add_library(LINK_LIBRARIES ipm::ipm msgpackc-cxx nlohmann_json::nlohmann_json)

##############################################################################

# Test applications
daq_add_application( network_object_send_receive network_object_send_receive.cxx TEST LINK_LIBRARIES serialization )
daq_add_application( serialization_speed serialization_speed.cxx TEST LINK_LIBRARIES serialization )
daq_add_application( serialization_speed_no_ipm serialization_speed_no_ipm.cxx TEST LINK_LIBRARIES serialization)
daq_add_application( non_moo_type non_moo_type.cxx TEST LINK_LIBRARIES serialization)
daq_add_application( inheritance inheritance.cxx TEST LINK_LIBRARIES serialization)

##############################################################################

daq_install()
