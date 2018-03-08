#
#
#


set( TARGET_NAME mymap_example )


set( EXT_LIBS mymap )


file(GLOB_RECURSE cpp_files *.c )


add_executable( ${TARGET_NAME} ${cpp_files} )
target_link_libraries( ${TARGET_NAME} ${EXT_LIBS} )
