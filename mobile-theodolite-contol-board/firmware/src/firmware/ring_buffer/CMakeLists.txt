set(SRCS ring_buffer.c)
set(HEADERS ring_buffer.h)
   
get_filename_component(package_name ${CMAKE_CURRENT_LIST_DIR} NAME)
add_library(${package_name} OBJECT ${SRCS} ${HEADERS})
