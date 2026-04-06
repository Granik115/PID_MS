add_executable(uart_link_test uart_link_test.cpp $<TARGET_OBJECTS:nanopb> $<TARGET_OBJECTS:link_level> $<TARGET_OBJECTS:ring_buffer>)
target_link_libraries(uart_link_test ${GTEST_BOTH_LIBRARIES})
add_test(uart_link_test uart_link_test)