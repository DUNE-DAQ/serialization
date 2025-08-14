cmake_minimum_required(VERSION 3.12)
project(serialization VERSION 2.1.0)

find_package(daq-cmake REQUIRED)
daq_setup_environment()

find_package(msgpack REQUIRED)
find_package(logging REQUIRED)
find_package(ers REQUIRED)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

##############################################################################
# Schema

##############################################################################
# Main library

# We don't have a real library, but we want to create a target for
# dependents to be able to depend on
daq_add_library(LINK_LIBRARIES msgpackc-cxx logging::logging)

##############################################################################

# Unit tests

daq_add_unit_test(Serialization_test  LINK_LIBRARIES serialization)

daq_install()
