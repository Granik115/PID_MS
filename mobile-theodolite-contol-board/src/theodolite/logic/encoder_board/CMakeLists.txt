set(SRCS encoder_board.cpp)
set(HEADERS encoder_board.h encoder_board_interface.h)
   
get_filename_component(package_name ${CMAKE_CURRENT_LIST_DIR} NAME)
add_library(${package_name} ${SRCS} ${HEADERS})


add_subdirectory(encoder_board_protocol)