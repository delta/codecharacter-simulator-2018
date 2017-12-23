cmake_minimum_required(VERSION 3.9.6)
project(llvm_pass)

set(SOURCE_FILES
	src/inst_count_pass.cpp
)

find_package(LLVM REQUIRED CONFIG)

if (${LLVM_PACKAGE_VERSION} VERSION_EQUAL "4.0.0")
	message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
else()
	message(FATAL_ERROR "Need LLVM version 4.0.0" )
endif()

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_library(instruction_count_pass MODULE ${SOURCE_FILES})

# LLVM is (typically) built with no C++ RTTI. We need to match that.
set_target_properties(instruction_count_pass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
if(APPLE)
    set_target_properties(instruction_count_pass PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)

install(TARGETS instruction_count_pass EXPORT instruction_count_pass_config
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)

install(EXPORT instruction_count_pass_config DESTINATION lib)
