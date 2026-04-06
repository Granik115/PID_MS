add_definitions("-Wno-sign-compare")

add_library(nmeap nmeap01.c)
target_include_directories(nmeap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
