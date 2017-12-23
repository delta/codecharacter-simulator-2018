cmake_minimum_required(VERSION 3.9.6)
project(main)

if((NOT BUILD_PROJECT STREQUAL "all") AND (NOT BUILD_PROJECT STREQUAL "no_tests"))
	include(${CMAKE_INSTALL_PREFIX}/lib/physics_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/state_config.cmake)
endif()

add_executable(simulator
  main.cpp)

target_link_libraries(simulator physics state)

install(TARGETS simulator DESTINATION bin)
