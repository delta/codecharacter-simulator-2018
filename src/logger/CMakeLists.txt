cmake_minimum_required(VERSION 3.9.6)
project(logger)

set(SOURCE_FILES
	src/logger.cpp
)

set(INCLUDE_PATH include)

set(EXPORTS_DIR ${CMAKE_BINARY_DIR}/exports)
set(EXPORTS_FILE_PATH ${EXPORTS_DIR}/logger/logger_export.h)

find_package(Protobuf REQUIRED)

file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto")
include_directories(${Protobuf_INCLUDE_DIRS})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${ProtoFiles})

if((NOT BUILD_PROJECT STREQUAL "all") AND (NOT BUILD_PROJECT STREQUAL "no_tests"))
	include(${CMAKE_INSTALL_PREFIX}/lib/physics_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/state_config.cmake)
endif()

add_library(logger SHARED ${SOURCE_FILES} ${PROTO_SRCS})
target_link_libraries(logger protobuf::libprotobuf state physics)

generate_export_header(logger EXPORT_FILE_NAME ${EXPORTS_FILE_PATH})

target_include_directories(logger PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}>
	$<BUILD_INTERFACE:${EXPORTS_DIR}>
	$<BUILD_INTERFACE:${Protobuf_INCLUDE_DIRS}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
	$<INSTALL_INTERFACE:include>
)

install(TARGETS logger EXPORT logger_config
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)

install(EXPORT logger_config DESTINATION lib)
install(DIRECTORY ${INCLUDE_PATH}/ DESTINATION include)
install(FILES ${PROTO_HDRS} DESTINATION include)
install(FILES ${EXPORTS_FILE_PATH} DESTINATION include/logger)
