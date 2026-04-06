set(SRCS ramp_control.cpp)
set(HEADERS ramp_control.h ramp_control_interface.h)
   
get_filename_component(package_name ${CMAKE_CURRENT_LIST_DIR} NAME)
add_library(${package_name} ${SRCS} ${HEADERS})
