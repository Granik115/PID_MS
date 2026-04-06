get_filename_component(package_name ${CMAKE_CURRENT_LIST_DIR} NAME)
add_executable("${package_name}_test" encoder_board_tst.cpp)
target_link_libraries("${package_name}_test" ${package_name} ${GTEST_BOTH_LIBRARIES} ${GMOCK_MAIN_LIBRARY} soft_timer firmware local_clock pc_protocol ${GMOCK_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME "${package_name}_test" COMMAND "${package_name}_test")
