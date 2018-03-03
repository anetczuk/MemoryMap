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
if (CMAKE_VERSION VERSION_LESS "3.1")
	## handled by gcc and clang
	set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99" )
else()
	## supported by CMake 3.1 and above
	set( CMAKE_C_STANDARD 99 )
	set( CMAKE_C_STANDARD_REQUIRED ON )
endif()

## compiler flags, gcc does not handle "appended" values
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-long-long" )


## enable CTest utility
enable_testing()



## ================= build =================


add_subdirectory( memorymap )

add_subdirectory( mymap )

