#
#
#

cmake_minimum_required(VERSION 3.0.2)


if( "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}" )
	message( FATAL_ERROR "Building in source is forbidden. Change output directory.")
endif()


project( memorymap )


if( NOT CMAKE_BUILD_TYPE )
	##set( CMAKE_BUILD_TYPE "Debug")
	set( CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set( CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Install path prefix" FORCE )
endif()


## set default compilation standard
set( CMAKE_C_STANDARD 90 )
set( CMAKE_C_STANDARD_REQUIRED ON )


## compiler flags
list(APPEND CMAKE_CXX_FLAGS "-Wall")
list(APPEND CMAKE_CXX_FLAGS "-pedantic")
#list(APPEND CMAKE_CXX_FLAGS "-Wno-long-long")


## enable CTest utility
enable_testing()


## ================= build =================

add_subdirectory( mymap )

