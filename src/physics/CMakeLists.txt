cmake_minimum_required(VERSION 3.6.2)
project(physics)

set(SOURCE_FILES
	src/vector.cpp
)

set(INCLUDE_PATH include)

set(EXPORTS_DIR ${CMAKE_BINARY_DIR}/exports)
set(EXPORTS_FILE_PATH ${EXPORTS_DIR}/physics/physics_export.h)

add_library(physics SHARED ${SOURCE_FILES})
target_link_libraries(physics)

generate_export_header(physics EXPORT_FILE_NAME ${EXPORTS_FILE_PATH})

target_include_directories(physics PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}>
	$<BUILD_INTERFACE:${EXPORTS_DIR}>
	$<INSTALL_INTERFACE:include>
)

install(TARGETS physics EXPORT physics_config
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)

install(EXPORT physics_config DESTINATION lib)
install(DIRECTORY ${INCLUDE_PATH}/ DESTINATION include)
install(FILES ${EXPORTS_FILE_PATH} DESTINATION include/physics)
