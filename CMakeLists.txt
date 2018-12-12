cmake_minimum_required (VERSION 2.4)

project (the_top)

set (SRCS_DIR srcs)
set (INCLUDE_DIRS includes)

find_package (Curses REQUIRED)

include_directories (${INCLUDE_DIRS} ${CURSES_INCLUDE_DIR})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -g")

add_library (SysInfo SHARED ${SRCS_DIR}/SysInfo.cpp)
add_library (Visual_ncs SHARED ${SRCS_DIR}/Visual_ncs.cpp)

add_executable (the_top ${SRCS_DIR}/main.cpp)
target_link_libraries (the_top SysInfo Visual_ncs ${CURSES_LIBRARIES})
