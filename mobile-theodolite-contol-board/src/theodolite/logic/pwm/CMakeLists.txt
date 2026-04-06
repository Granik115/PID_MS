set(SRCS motor_pwm.cpp)
set(HEADERS motor_pwm.h pwm_interface.h)
   
get_filename_component(package_name ${CMAKE_CURRENT_LIST_DIR} NAME)
add_library(${package_name} ${SRCS} ${HEADERS})
