cmake_minimum_required(VERSION 3.6.2)
project(tests)

set(BUILD_ROOT_DIR ${CMAKE_BINARY_DIR})

set(SOURCE_FILES
	test_main.cpp
	state/state_test.cpp
	state/soldier_test.cpp
)

if(NOT BUILD_PROJECT STREQUAL "all")
	include(${CMAKE_INSTALL_PREFIX}/lib/physics_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/state_config.cmake)
endif()

add_executable(tests ${SOURCE_FILES})

target_link_libraries(tests physics state gtest)

install(TARGETS tests DESTINATION bin)
