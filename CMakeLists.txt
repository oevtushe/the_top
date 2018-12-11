cmake_minimum_required (VERSION 3.7)

project (the_top)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

add_library (SysInfo SHARED SysInfo.cpp)
add_library (Visual_ncs SHARED Visual_ncs.cpp)

add_executable (the_top main.cpp)
target_link_libraries (the_top SysInfo ncurses Visual_ncs)
