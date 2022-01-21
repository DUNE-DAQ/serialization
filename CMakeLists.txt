cmake_minimum_required(VERSION 3.12)
project(serialization VERSION 1.2.3)

find_package(daq-cmake REQUIRED)
daq_setup_environment()

find_package(nlohmann_json REQUIRED)
find_package(msgpack REQUIRED)
find_package(logging REQUIRED)
find_package(ers REQUIRED)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

##############################################################################
# Schema

daq_codegen(*.jsonnet TEST TEMPLATES Structs.hpp.j2 Nljs.hpp.j2 MsgP.hpp.j2)

##############################################################################
# Main library

# We don't have a real library, but we want to create a target for
# dependents to be able to depend on
daq_add_library(LINK_LIBRARIES msgpackc-cxx nlohmann_json::nlohmann_json logging::logging)

##############################################################################

# Test applications
daq_add_application( serialization_speed_no_ipm serialization_speed_no_ipm.cxx TEST LINK_LIBRARIES serialization)
daq_add_application( non_moo_type non_moo_type.cxx TEST LINK_LIBRARIES serialization)
daq_add_application( inheritance inheritance.cxx TEST LINK_LIBRARIES serialization)

##############################################################################

# Unit tests

daq_add_unit_test(Serialization_test  LINK_LIBRARIES serialization)

daq_install()
