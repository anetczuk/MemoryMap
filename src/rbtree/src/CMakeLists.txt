#
#
#


set( TARGET_NAME rbtree )


set( EXT_LIBS  )


file(GLOB_RECURSE cpp_files *.c )


add_library( ${TARGET_NAME} SHARED ${cpp_files} )
target_link_libraries( ${TARGET_NAME} ${EXT_LIBS} )
