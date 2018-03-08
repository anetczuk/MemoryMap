#
#
#


set( TARGET_NAME mymap )


include_directories( "../../memorymap/include" )


set( EXT_LIBS memorymap )


file(GLOB_RECURSE cpp_files *.c )


add_library( ${TARGET_NAME} SHARED ${cpp_files} )
target_link_libraries( ${TARGET_NAME} ${EXT_LIBS} )
