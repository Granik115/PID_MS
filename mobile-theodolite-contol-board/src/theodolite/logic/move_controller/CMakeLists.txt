set(SRCS move_controller.cpp)
set(HEADERS move_controller.h move_controller_interface.h)
   
get_filename_component(package_name ${CMAKE_CURRENT_LIST_DIR} NAME)
add_library(${package_name} ${SRCS} ${HEADERS})
