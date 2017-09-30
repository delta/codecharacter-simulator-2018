cmake_minimum_required(VERSION 3.6.2)
project(state)

set(SOURCE_FILES
	src/state.cpp
	src/actor/actor.cpp
	src/actor/soldier.cpp
	src/actor/tower.cpp
	src/actor/respawn_system/respawn_system.cpp
	src/actor/respawn_system/respawn_system_static_init.cpp
	src/map/map.cpp
	src/map/map_element.cpp
	src/money_manager/money_manager.cpp
	src/tower_manager/tower_manager.cpp
	src/state_syncer/state_syncer.cpp
)

set(INCLUDE_PATH include)

set(EXPORTS_DIR ${CMAKE_BINARY_DIR}/exports)
set(EXPORTS_FILE_PATH ${EXPORTS_DIR}/state/state_export.h)

if((NOT BUILD_PROJECT STREQUAL "all") AND (NOT BUILD_PROJECT STREQUAL "no_tests"))
	include(${CMAKE_INSTALL_PREFIX}/lib/physics_config.cmake)
endif()

add_library(state SHARED ${SOURCE_FILES})
target_link_libraries(state physics)

generate_export_header(state EXPORT_FILE_NAME ${EXPORTS_FILE_PATH})

target_include_directories(state PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}>
	$<BUILD_INTERFACE:${EXPORTS_DIR}>
	$<INSTALL_INTERFACE:include/state>
)

install(TARGETS state EXPORT state_config
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)

install(EXPORT state_config DESTINATION lib)
install(DIRECTORY ${INCLUDE_PATH}/ DESTINATION include)
install(FILES ${EXPORTS_FILE_PATH} DESTINATION include/state)
