add_executable(RingBufferTest RingBuffer_Test.cpp $<TARGET_OBJECTS:ring_buffer>)
target_link_libraries(RingBufferTest ${GTEST_BOTH_LIBRARIES})
add_test(RingBufferTest RingBufferTest)
