cmake_minimum_required(VERSION 3.6.2)
project(llvm_pass)

set(SOURCE_FILES
	src/inst_count_pass.cpp
)

find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_library(DynamicInstructionCountPass MODULE ${SOURCE_FILES})

# LLVM is (typically) built with no C++ RTTI. We need to match that.
set_target_properties(DynamicInstructionCountPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
if(APPLE)
    set_target_properties(DynamicInstructionCountPass PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)
