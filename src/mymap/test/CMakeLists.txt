#
#
#


find_package(CMOCKA 0.4)


if (NOT CMOCKA_FOUND)
	message( WARNING "No CMocka found. Try running 'sudo aptitude install libcmocka-dev'. Compilation without unit tests." )
	return() 
endif()


include_directories( ${CMOCKA_INCLUDE_DIR} )


set( EXT_LIBS mymap ${CMOCKA_LIBRARIES} )


add_executable( mymap_test MyMap_test.c )
target_link_libraries( mymap_test ${EXT_LIBS} )

add_executable( linkedlist_test LinkedList_test.c )
target_link_libraries( linkedlist_test ${EXT_LIBS} )
