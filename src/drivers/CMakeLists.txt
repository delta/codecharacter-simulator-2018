cmake_minimum_required(VERSION 3.6.2)
project(drivers)

set(SOURCE_FILES
	src/shared_memory_utils/shared_memory_main.cpp
	src/shared_memory_utils/shared_memory_player.cpp
	src/shared_memory_utils/shared_buffer.cpp
	src/timer.cpp
	src/main_driver.cpp
	src/player_driver.cpp
)

set(INCLUDE_PATH include)

set(EXPORTS_DIR ${CMAKE_BINARY_DIR}/exports)
set(EXPORTS_FILE_PATH ${EXPORTS_DIR}/drivers/drivers_export.h)

if((NOT BUILD_PROJECT STREQUAL "all") AND (NOT BUILD_PROJECT STREQUAL "no_tests"))
	include(${CMAKE_INSTALL_PREFIX}/lib/physics_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/player_wrapper_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/state_config.cmake)
endif()

add_library(drivers SHARED ${SOURCE_FILES})

if(UNIX AND NOT APPLE)
	target_link_libraries(drivers physics player_wrapper state rt)
else()
	target_link_libraries(drivers physics player_wrapper state)
endif()

generate_export_header(drivers EXPORT_FILE_NAME ${EXPORTS_FILE_PATH})

target_include_directories(drivers PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}>
	$<BUILD_INTERFACE:${EXPORTS_DIR}>
	$<BUILD_INTERFACE:${BOOST_INCLUDE_PATH}>
	$<INSTALL_INTERFACE:include>
)

install(TARGETS drivers EXPORT drivers_config
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)

install(EXPORT drivers_config DESTINATION lib)
install(DIRECTORY ${INCLUDE_PATH}/ DESTINATION include)
install(FILES ${EXPORTS_FILE_PATH} DESTINATION include/drivers)
