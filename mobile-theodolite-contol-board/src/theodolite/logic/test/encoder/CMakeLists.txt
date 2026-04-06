get_filename_component(package_name ${CMAKE_CURRENT_LIST_DIR} NAME)
add_executable("${package_name}_test" encoder_tst.cpp)
target_link_libraries("${package_name}_test" ${package_name} encoder ${GTEST_BOTH_LIBRARIES} ${GMOCK_MAIN_LIBRARY} ${GMOCK_LIBRARY} )
add_test(NAME "${package_name}_test" COMMAND "${package_name}_test")
