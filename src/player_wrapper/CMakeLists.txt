cmake_minimum_required(VERSION 3.9.6)
project(player_wrapper)

set(SOURCE_FILES
  src/player_code_wrapper.cpp
)

set(INCLUDE_PATH include)

set(EXPORTS_DIR ${CMAKE_BINARY_DIR}/exports)
set(EXPORTS_FILE_PATH ${EXPORTS_DIR}/player_wrapper/player_wrapper_export.h)

add_library(player_wrapper SHARED ${SOURCE_FILES})

target_link_libraries(player_wrapper state)

generate_export_header(player_wrapper EXPORT_FILE_NAME ${EXPORTS_FILE_PATH})

target_include_directories(player_wrapper PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}>
	$<BUILD_INTERFACE:${EXPORTS_DIR}>
	$<INSTALL_INTERFACE:include>
)

install(TARGETS player_wrapper EXPORT player_wrapper_config
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)

install(EXPORT player_wrapper_config DESTINATION lib)
install(DIRECTORY ${INCLUDE_PATH}/ DESTINATION include)
install(FILES ${EXPORTS_FILE_PATH} DESTINATION include/player_wrapper)
