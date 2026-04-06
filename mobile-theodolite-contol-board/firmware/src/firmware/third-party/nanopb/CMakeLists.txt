set(SRCS pb_common.c pb_common.h pb_decode.c pb_decode.h pb_encode.c pb_encode.h pb.h)

get_filename_component(package_name ${CMAKE_CURRENT_LIST_DIR} NAME)
add_library(${package_name} OBJECT ${SRCS} ${HEADERS})
