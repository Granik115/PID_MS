set(SRCS sw_uart.cpp)
set(HEADERS sw_uart.h)
   
get_filename_component(package_name ${CMAKE_CURRENT_LIST_DIR} NAME)
add_library(${package_name} ${SRCS} ${HEADERS})
