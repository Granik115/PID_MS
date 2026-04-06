get_filename_component(package_name ${CMAKE_CURRENT_LIST_DIR} NAME)
add_executable("${package_name}_test" pwm_test.cpp)
target_link_libraries("${package_name}_test" ${package_name} firmware ${GTEST_BOTH_LIBRARIES} ${GMOCK_MAIN_LIBRARY} ${GMOCK_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ramp_control)
add_test(NAME "${package_name}_test" COMMAND "${package_name}_test")