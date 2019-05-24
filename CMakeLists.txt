cmake_minimum_required(VERSION 3.11)
project(HashComp)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost 1.45.0)

include_directories( ${Boost_INCLUDE_DIRS} )
include_directories(../neatlib)

set(EBR_C_FILES ../neatlib/epoch/faster/thread.cc ../neatlib/epoch/faster/lss_allocator.cc)
add_subdirectory(../neatlib "${CMAKE_CURRENT_BINARY_DIR}/neatlib")
add_executable(HashComp main.cpp ${LEVEL_HASH_C_FILES} ${EBR_C_FILES})
if (UNIX)
    target_link_libraries(HashComp pthread)
endif()
