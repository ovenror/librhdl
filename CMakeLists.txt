cmake_minimum_required(VERSION 3.13)

project(librhdl VERSION 1.0.0 DESCRIPTION "hardware synthesis library for Minecraft Redstone circuits")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Boost 1.71)

IF (NOT Boost_FOUND)
    message(FATAL_ERROR "Need Boost library headers. Please install libboost-dev >= 1.71")
endif()

add_subdirectory(examples)

add_library(rhdl_obj OBJECT "")
target_include_directories(rhdl_obj PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(rhdl_obj PRIVATE src)
target_include_directories(rhdl_obj PUBLIC include)
target_compile_features(rhdl_obj PRIVATE cxx_std_17)

if(MSVC)
    add_definitions(/FI"${PROJECT_SOURCE_DIR}/src/util/debug.h")
else()
    # GCC or Clang
    add_definitions(-include ${PROJECT_SOURCE_DIR}/src/util/debug.h)
endif()

option(sharedLib "Build rhdl library as a shared library" OFF)

if (sharedLib)
	set_property(TARGET rhdl_obj PROPERTY POSITION_INDEPENDENT_CODE 1)
	add_library(rhdl SHARED $<TARGET_OBJECTS:rhdl_obj>)
else()
	add_library(rhdl $<TARGET_OBJECTS:rhdl_obj>)
endif()
target_include_directories(rhdl PUBLIC include)

add_subdirectory(src)

enable_testing()
add_subdirectory(test)

