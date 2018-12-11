cmake_minimum_required (VERSION 3.7)

project (the_top)

set (SRCS_DIR srcs)
set (INCLUDE_DIRS includes)

include_directories (${INCLUDE_DIRS})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library (SysInfo SHARED ${SRCS_DIR}/SysInfo.cpp)
add_library (Visual_ncs SHARED ${SRCS_DIR}/Visual_ncs.cpp)

add_executable (the_top ${SRCS_DIR}/main.cpp)
target_link_libraries (the_top SysInfo ncurses Visual_ncs)
