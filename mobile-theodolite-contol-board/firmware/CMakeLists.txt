cmake_minimum_required(VERSION 3.9.0)
cmake_policy(SET CMP0016 NEW)

project(firmware VERSION 0.0.0 DESCRIPTION "Библиотека общий решений встраиваемых систем")

set(ARCHS x86_64-redhat-linux-8)

set(RELEASE_MAIL_TO "сpp-libs-firmware-developers@nicetu.spb.ru")

### INCLUDE BUILDSYSTEM ############################################################################

option(USE_DEPPACKAGES "Use packages as dependencies" OFF)

if(USE_DEPPACKAGES)
  find_package(build-system 4.1.3 REQUIRED)
else()
  set(BUILD_SYSTEM_COMMON_PATH "${PROJECT_SOURCE_DIR}/.cmake")
  if(NOT IS_DIRECTORY ${BUILD_SYSTEM_COMMON_PATH})
    find_package(Git REQUIRED)
    set(git_client ${GIT_EXECUTABLE})
    execute_process(COMMAND ${git_client} clone --branch 4.1.3 --depth 1 http://git/cpp-libs/build-system.git ${BUILD_SYSTEM_COMMON_PATH} RESULT_VARIABLE child_result)
    if(NOT child_result EQUAL "0")
      message(FATAL_ERROR "git checkout failed. Error(${child_result})")
    endif()
    file(REMOVE_RECURSE ${BUILD_SYSTEM_COMMON_PATH}/.git)
  endif()
  include(${BUILD_SYSTEM_COMMON_PATH}/meta_include.cmake)
endif()

include(GNUInstallDirs)

### SOURCE SPECIFIC ################################################################################

nic__define_platform(PLATFORM PLATFORM_VERSION)

set(CPACK_RPM_BUILDREQUIRES "build-system >= ${build-system_VERSION}")

option(WITH_TESTS "Include test tools in build requires" OFF)

if(WITH_TESTS)
  string(CONCAT CPACK_RPM_BUILDREQUIRES ${CPACK_RPM_BUILDREQUIRES} ", gtest-devel, gmock-devel")
endif()

set(CPACK_RPM_SOURCE_PKG_BUILD_PARAMS "${CPACK_RPM_SOURCE_PKG_BUILD_PARAMS} -DUSE_DEPPACKAGES=On")

nic__cache_buildrequires_for_install_dependencies()

### MAKING PRODUCTS ################################################################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(src/firmware/link_level)
add_subdirectory(src/firmware/ring_buffer)
add_subdirectory(src/firmware/third-party/nanopb)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(firmware STATIC  $<TARGET_OBJECTS:link_level> $<TARGET_OBJECTS:ring_buffer> $<TARGET_OBJECTS:nanopb>)

if(WITH_TESTS)
  find_package(GTest REQUIRED)
  include_directories(${GTEST_INCLUDE_DIR})

  enable_testing()

  add_subdirectory(test)
endif()
