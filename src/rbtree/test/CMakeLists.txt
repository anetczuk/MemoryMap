#
#
#


find_package(CMOCKA 0.4)


if (NOT CMOCKA_FOUND)
	message( WARNING "No CMocka found. Try running 'sudo aptitude install libcmocka-dev'. Compilation without unit tests." )
	return() 
endif()


include_directories( ${CMOCKA_INCLUDE_DIR} )


set( EXT_LIBS rbtree ${CMOCKA_LIBRARIES} )


file(GLOB cpp_test_files *_test.c )


## generate test suites
foreach(test_filename IN LISTS cpp_test_files)
	## extract filename without extension
	get_filename_component(test_name ${test_filename} NAME_WE)

	## build executable
	add_executable( ${test_name} ${test_filename} )
	target_link_libraries( ${test_name} ${EXT_LIBS} )
	add_test( memorymap/${test_name} ${test_name} )
	
	## generate bash script
	generate_file( "${CMAKE_SOURCE_DIR}/runSuite.sh.in" "test_${test_name}.sh" )
endforeach()
