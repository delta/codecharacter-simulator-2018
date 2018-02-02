cmake_minimum_required(VERSION 3.9.6)
project(player_code)

set(PLAYER_ID "0" CACHE STRING "Specifies the player ID. Should be between 1
    and NUM_PLAYERS to build as that player, 0 to build as all players")

file(GLOB SOURCE_FILES src/*.cpp)

set(CMAKE_CXX_FLAGS "-Werror=return-type")

set(INCLUDE_PATH include)

set(EXPORTS_DIR ${CMAKE_BINARY_DIR}/exports)
set(EXPORTS_FILE_PATH ${EXPORTS_DIR}/player_code/player_code_export.h)

if((NOT BUILD_PROJECT STREQUAL "all") AND (NOT BUILD_PROJECT STREQUAL "no_tests"))
	include(${CMAKE_INSTALL_PREFIX}/lib/physics_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/state_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/player_wrapper_config.cmake)
endif()

if(BUILD_PROJECT STREQUAL "player_code")
	set(LLVM_PASS_PATH ${CMAKE_INSTALL_PREFIX}/lib/libinstruction_count_pass.so)
else()
	set(LLVM_PASS_PATH ${CMAKE_BINARY_DIR}/lib/libinstruction_count_pass.so)
endif()

function(instrument_and_install_lib TARGET_NAME)
	if(NOT BUILD_PROJECT STREQUAL "player_code")
		add_dependencies(${TARGET_NAME} instruction_count_pass)
	endif()
	target_link_libraries(${TARGET_NAME} state)
	set_target_properties(${TARGET_NAME} PROPERTIES
		COMPILE_FLAGS "-Xclang -load -Xclang ${LLVM_PASS_PATH} -c -fPIC")

	install(TARGETS ${TARGET_NAME} EXPORT ${TARGET_NAME}_config DESTINATION lib)
	target_include_directories(${TARGET_NAME} PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}>
		$<BUILD_INTERFACE:${EXPORTS_DIR}>
	)

	if((NOT BUILD_PROJECT STREQUAL "all") AND (NOT BUILD_PROJECT STREQUAL "no_tests"))
		target_include_directories(${TARGET_NAME} PUBLIC
			$<BUILD_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
		)
	else()
		target_include_directories(${TARGET_NAME} PUBLIC
			$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/player_wrapper/include>
		)
	endif()

endfunction(instrument_and_install_lib)

function(build_player_code TARGET_NAME)

	add_library(${TARGET_NAME}_code SHARED ${SOURCE_FILES})

	generate_export_header(${TARGET_NAME}_code EXPORT_FILE_NAME ${EXPORTS_FILE_PATH} EXPORT_MACRO_NAME "PLAYER_CODE_EXPORT")

	instrument_and_install_lib(${TARGET_NAME}_code)

	target_include_directories(${TARGET_NAME}_code PUBLIC
		$<INSTALL_INTERFACE:include>
	)

	install(EXPORT ${TARGET_NAME}_code_config DESTINATION lib)

endfunction(build_player_code)

if((BUILD_PROJECT STREQUAL "player_code") AND (NOT PLAYER_ID STREQUAL "0"))
	build_player_code("player_${PLAYER_ID}")
else()
	foreach(ITER 1 ${NUM_PLAYERS})
		build_player_code("player_${ITER}")
	endforeach(ITER)
endif()

install(DIRECTORY ${INCLUDE_PATH}/ DESTINATION include)
install(FILES ${EXPORTS_FILE_PATH} DESTINATION include/player_code)

if((NOT BUILD_PROJECT STREQUAL "player_code") AND (NOT BUILD_PROJECT STREQUAL "no_tests"))
	set(PLAYER_CODE_TEST_COUNT 0)
	file(GLOB TEST_FILES test/*.cpp)

	foreach(FILE ${TEST_FILES})
		add_library(player_code_test_${PLAYER_CODE_TEST_COUNT} SHARED ${FILE})
		instrument_and_install_lib(player_code_test_${PLAYER_CODE_TEST_COUNT})
		math(EXPR PLAYER_CODE_TEST_COUNT "${PLAYER_CODE_TEST_COUNT} + 1")
	endforeach()
endif()
