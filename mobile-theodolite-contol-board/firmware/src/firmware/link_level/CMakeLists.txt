set(SRCS uart_link.cpp crc16.cpp)
set(HEADERS uart_link.h crc16.h)
   
get_filename_component(package_name ${CMAKE_CURRENT_LIST_DIR} NAME)
add_library(${package_name} OBJECT ${SRCS} ${HEADERS})
