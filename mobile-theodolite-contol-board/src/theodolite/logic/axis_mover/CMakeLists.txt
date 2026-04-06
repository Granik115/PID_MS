set(SRCS axis_mover.cpp)
set(HEADERS axis_mover.h axis_mover_interface.h)
   
get_filename_component(package_name ${CMAKE_CURRENT_LIST_DIR} NAME)
add_library(${package_name} ${SRCS} ${HEADERS})
