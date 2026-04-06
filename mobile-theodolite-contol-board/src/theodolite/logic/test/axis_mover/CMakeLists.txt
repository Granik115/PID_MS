get_filename_component(package_name ${CMAKE_CURRENT_LIST_DIR} NAME)
add_executable("${package_name}_test" axis_mover_test.cpp)
target_link_libraries("${package_name}_test" ${package_name} firmware ${GTEST_BOTH_LIBRARIES} ${GMOCK_MAIN_LIBRARY} ${GMOCK_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ramp_control encoder_board pid_regulator brakes soft_timer local_clock)
add_test(NAME "${package_name}_test" COMMAND "${package_name}_test")