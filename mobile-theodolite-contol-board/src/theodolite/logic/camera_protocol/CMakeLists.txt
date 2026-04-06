set(SRCS camera.cpp)
set(HEADERS camera_interface.h camera.h)
   
get_filename_component(package_name ${CMAKE_CURRENT_LIST_DIR} NAME)
add_library(${package_name} ${SRCS} ${HEADERS})
