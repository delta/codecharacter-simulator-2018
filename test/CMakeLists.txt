cmake_minimum_required(VERSION 3.9.6)
project(tests)

find_package(Boost 1.68.0)

include_directories(.)
include_directories(${Boost_INCLUDE_DIRS})

set(SOURCE_FILES
	test_main.cpp
	state/mocks/map_mock.h
	state/tower_test.cpp
	state/map_test.cpp
	state/money_manager_test.cpp
	state/tower_manager_test.cpp
	state/soldier_test.cpp
	state/path_planner_test.cpp
	state/simple_path_planner_test.cpp
	state/state_syncer_test.cpp
	drivers/shared_memory/shm_test.cpp
	drivers/timer_test.cpp
	drivers/main_driver_test.cpp
	llvm_pass/llvm_pass_test.cpp
	logger/logger_test.cpp
)

if(NOT BUILD_PROJECT STREQUAL "all")
	include(${CMAKE_INSTALL_PREFIX}/lib/physics_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/state_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/drivers_config.cmake)
endif()

add_executable(tests ${SOURCE_FILES})

add_executable(shm_client drivers/shared_memory/shm_client.cpp)

add_executable(main_driver_test_player drivers/main_driver_test_player)

target_link_libraries(tests physics state logger drivers player_wrapper gtest gmock)
target_link_libraries(tests player_code_test_0 player_code_test_1 player_code_test_2)

target_link_libraries(shm_client state drivers)

target_link_libraries(main_driver_test_player drivers)

install(TARGETS tests shm_client main_driver_test_player DESTINATION bin)
