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
set_target_properties (the_top PROPERTIES
	CMAKE_CXX_STANDARD			11
	CMAKE_CXX_STANDARD_REQUIRED TRUE
	CMAKE_CXX_FLAGS				"${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -pthread"
)
add_library (SysInfo SHARED
		srcs/SysInfo.cpp
		srcs/ISys.cpp
)
add_library (Visual_ncs SHARED
		srcs/Visual_ncs.cpp
		srcs/ProcessWindow.cpp
		srcs/SignalsWindow.cpp
		srcs/MetersWindow.cpp
		srcs/TextInfoWindow.cpp
		srcs/Selectable.cpp
		srcs/Window.cpp
)
target_link_libraries (${CMAKE_PROJECT_NAME}
		SysInfo
		Visual_ncs
		${CURSES_LIBRARIES}
		pthread
)
