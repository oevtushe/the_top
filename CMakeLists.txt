cmake_minimum_required (VERSION 3.1.3)

project (the_top)

find_package (Curses REQUIRED)

add_executable (${CMAKE_PROJECT_NAME}
	srcs/main.cpp
)
include_directories (
	includes
)
target_include_directories (${CMAKE_PROJECT_NAME}
	PUBLIC 
		srcs
	PRIVATE
		${CURSES_INCLUDE_DIR}
)

#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_FLAGS	"${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -pthread -std=c++11")

add_library (SysInfo SHARED
		srcs/SysInfo.cpp
		srcs/ISys.cpp
)
add_library (Visual_htop_ncs SHARED
		srcs/Visual_htop_ncs.cpp
		srcs/ProcessWindow.cpp
		srcs/SignalsWindow.cpp
		srcs/MetersWindow.cpp
		srcs/TextInfoWindow.cpp
		srcs/Selectable.cpp
		srcs/Window.cpp
)
add_library (Visual_top_ncs SHARED
		srcs/Visual_top_ncs.cpp
)
target_link_libraries (${CMAKE_PROJECT_NAME}
		SysInfo
		Visual_htop_ncs
		Visual_top_ncs
		${CURSES_LIBRARIES}
		pthread
)
