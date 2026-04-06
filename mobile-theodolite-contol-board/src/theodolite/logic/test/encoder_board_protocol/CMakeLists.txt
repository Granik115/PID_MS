get_filename_component(package_name ${CMAKE_CURRENT_LIST_DIR} NAME)
add_executable("${package_name}_test" encoder_protocol_tst.cpp)
target_link_libraries("${package_name}_test" ${package_name} ${GTEST_BOTH_LIBRARIES} firmware encoder_board encoder_board_protocol  soft_timer local_clock pc_protocol ${GMOCK_MAIN_LIBRARY} ${GMOCK_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME "${package_name}_test" COMMAND "${package_name}_test")

