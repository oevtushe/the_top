cmake_minimum_required (VERSION 3.1.3)

project (the_top)

set (SRCS_DIR ${CMAKE_SOURCE_DIR}/srcs)
set (INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/includes)

find_package (Curses REQUIRED)

include_directories (${INCLUDE_DIRS} ${CURSES_INCLUDE_DIR})

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED TRUE)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -pthread")

add_library (SysInfo SHARED
		${SRCS_DIR}/SysInfo.cpp
		${SRCS_DIR}/ISys.cpp
)
add_library (Visual_ncs SHARED
		${SRCS_DIR}/Visual_ncs.cpp
)

add_executable (${CMAKE_PROJECT_NAME} ${SRCS_DIR}/main.cpp)
target_link_libraries (${CMAKE_PROJECT_NAME}
		SysInfo
		Visual_ncs
		${CURSES_LIBRARIES})
