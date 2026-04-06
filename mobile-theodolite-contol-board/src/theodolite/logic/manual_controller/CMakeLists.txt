set(SRCS manual_controller.cpp)
set(HEADERS manual_controller.h manual_controller_interface.h)
   
get_filename_component(package_name ${CMAKE_CURRENT_LIST_DIR} NAME)
add_library(${package_name} ${SRCS} ${HEADERS})
