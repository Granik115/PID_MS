set(SRCS sw_timer.cpp)
set(HEADERS sw_timer.h)
   
get_filename_component(package_name ${CMAKE_CURRENT_LIST_DIR} NAME)
add_library(${package_name} ${SRCS} ${HEADERS})
