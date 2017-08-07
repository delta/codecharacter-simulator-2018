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
	drivers/shared_memory/shm_test.cpp
)

if(NOT BUILD_PROJECT STREQUAL "all")
	include(${CMAKE_INSTALL_PREFIX}/lib/physics_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/state_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/drivers_config.cmake)
endif()

add_executable(tests ${SOURCE_FILES})

add_executable(shm_client drivers/shared_memory/shm_client.cpp)

target_link_libraries(tests physics state drivers gtest gmock)

target_link_libraries(shm_client state drivers)

install(TARGETS tests shm_client DESTINATION bin)
