cmake_minimum_required(VERSION 3.6.2)
project(tests)

include_directories(.)

set(SOURCE_FILES
	test_main.cpp
	state/mocks/map_mock.h
	state/tower_test.cpp
	state/map_test.cpp
	state/money_manager_test.cpp
	state/tower_manager_test.cpp
	state/soldier_test.cpp
)

if(NOT BUILD_PROJECT STREQUAL "all")
	include(${CMAKE_INSTALL_PREFIX}/lib/physics_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/state_config.cmake)
endif()

add_executable(tests ${SOURCE_FILES})

target_link_libraries(tests physics state gtest gmock)

install(TARGETS tests DESTINATION bin)
