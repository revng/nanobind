# This is a custom CMakeLists that allows building the nanobind .cpp files into
# the libnanobind.so library. This allows building and packaging nanobind in
# the "traditional" way of linux packages (with libnanobind.so in /usr/lib and
# headers in /usr/include). It also avoid having to import nanobind's cmake in
# the downstream project, all it's needed is to build the python module and
# link it against libnanobind.

cmake_minimum_required(VERSION 3.15)
project(nanobind-standlone)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

find_package(Python REQUIRED COMPONENTS Interpreter Development)
include(../cmake/nanobind-config.cmake)
nanobind_build_library("nanobind" NOSTRIP NOLTO)
target_compile_definitions(nanobind PRIVATE -DNB_BUILD)
target_compile_definitions(nanobind PUBLIC -DNB_SHARED)
add_custom_target(build-nanobind ALL DEPENDS nanobind)
install(TARGETS nanobind LIBRARY)

file(RELATIVE_PATH PYTHON_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}"
     "${Python_SITELIB}")

file(GLOB PYTHON_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../src/*.py")
install(
  FILES ${PYTHON_FILES}
  DESTINATION "${CMAKE_INSTALL_PREFIX}/${PYTHON_INSTALL_PATH}/nanobind")

file(
  GLOB_RECURSE HEADER_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/../include"
  LIST_DIRECTORIES false
  "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h")

foreach(HEADER_FILE ${HEADER_FILES})
  get_filename_component(HEADER_DIR "${HEADER_FILE}" DIRECTORY)
  install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/../include/${HEADER_FILE}"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${HEADER_DIR}")
endforeach()
