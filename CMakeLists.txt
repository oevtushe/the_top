project (the_top)

set (SRCS SysInfo.cpp main.cpp Visual_ncs.cpp The_top.cpp)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_executable (the_top ${SRCS})
target_link_libraries (the_top ncurses)
