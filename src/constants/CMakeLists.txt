cmake_minimum_required(VERSION 3.9.6)
project(constants)

set(INCLUDE_PATH include)

add_library(constants INTERFACE)
target_link_libraries(constants INTERFACE physics)

target_include_directories(constants INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}>
	$<INSTALL_INTERFACE:include>
)

install(TARGETS constants EXPORT constants_config
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)

install(EXPORT constants_config DESTINATION lib)
install(DIRECTORY ${INCLUDE_PATH}/ DESTINATION include)
