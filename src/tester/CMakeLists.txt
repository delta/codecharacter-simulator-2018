cmake_minimum_required(VERSION 3.6.2)
project(tester)

set(BUILD_ROOT_DIR ${CMAKE_BINARY_DIR})

if(NOT BUILD_PROJECT STREQUAL "all")
	include(${CMAKE_INSTALL_PREFIX}/lib/physics_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/state_config.cmake)
endif()

add_executable(tester
  tester.cpp)

target_link_libraries(tester physics state gtest)

install(TARGETS tester DESTINATION bin)
