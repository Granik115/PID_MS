set(SRCS pid.cpp)
set(HEADERS pid.h pid_regulator_interface.h)
   
get_filename_component(package_name ${CMAKE_CURRENT_LIST_DIR} NAME)
add_library(${package_name} ${SRCS} ${HEADERS})
