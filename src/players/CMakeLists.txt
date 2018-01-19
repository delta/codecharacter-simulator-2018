cmake_minimum_required(VERSION 3.9.6)
project(players)

set(SOURCE_FILES
	player.cpp
)

set(INCLUDE_PATH include)

if((NOT BUILD_PROJECT STREQUAL "all") AND (NOT BUILD_PROJECT STREQUAL "no_tests"))
	include(${CMAKE_INSTALL_PREFIX}/lib/physics_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/constants_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/player_wrapper_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/state_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/logger_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/drivers_config.cmake)
endif()

foreach(PLAYER_ID 1 ${NUM_PLAYERS})
	set(PLAYER_NAME "player_${PLAYER_ID}")

	if((NOT BUILD_PROJECT STREQUAL "all") AND (NOT BUILD_PROJECT STREQUAL "no_tests"))
		include(${CMAKE_INSTALL_PREFIX}/lib/${PLAYER_NAME}_config.cmake)
	endif()


	add_executable(${PLAYER_NAME} ${SOURCE_FILES})
	target_link_libraries(${PLAYER_NAME} physics state logger drivers player_wrapper ${PLAYER_NAME}_code constants)

	target_include_directories(${PLAYER_NAME} PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}>
		$<INSTALL_INTERFACE:include>
	)

	install(TARGETS ${PLAYER_NAME}
		ARCHIVE DESTINATION lib
		LIBRARY DESTINATION lib
		RUNTIME DESTINATION bin
	)

endforeach(PLAYER_ID)
