set(SRCS protocol_processor.cpp third-party/theodolite_messages.pb.c third-party/timesync-messages.pb.c)
set(HEADERS protocol_processor_interface.h protocol_processor.h third-party/theodolite_messages.pb.h third-party/timesync-messages.pb.h)
   
get_filename_component(package_name ${CMAKE_CURRENT_LIST_DIR} NAME)
add_library(${package_name} ${SRCS} ${HEADERS})
target_link_libraries(${package_name} firmware)
