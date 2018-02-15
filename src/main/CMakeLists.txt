cmake_minimum_required(VERSION 3.9.6)
project(main)

set(SOURCE_FILES
	main.cpp
)

set(INCLUDE_PATH include)

set (Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.64.0 EXACT REQUIRED COMPONENTS system)

if((NOT BUILD_PROJECT STREQUAL "all") AND (NOT BUILD_PROJECT STREQUAL "no_tests"))
	include(${CMAKE_INSTALL_PREFIX}/lib/physics_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/constants_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/player_wrapper_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/state_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/logger_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/drivers_config.cmake)
endif()

add_executable(main ${SOURCE_FILES})
target_link_libraries(main physics state logger drivers player_wrapper constants Boost::system)

if (UNIX)
	target_link_libraries(main pthread)
endif()

target_include_directories(main PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}>
	$<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
	$<INSTALL_INTERFACE:include>
)

install(TARGETS main
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)
