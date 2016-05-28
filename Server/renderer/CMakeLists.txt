cmake_minimum_required( VERSION 2.8 )

project( muskat )

set( CMAKE_BUILD_TYPE "RELEASE" )
set( CMAKE_CXX_FLAGS "-Wall -O2 -std=c++11" )
add_definitions( ${CMAKE_CXX_FLAGS} )

find_package( OpenMP )
if( OPENMP_FOUND) 
	set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" )
	set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
endif()

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
	include_directories(${GLEW_INCLUDE_DIRS})
	link_libraries(${GLEW_LIBRARIES})
endif()

find_package(GLUT REQUIRED)
if (GLUT_FOUND)
	include_directories(${GLUT_INCLUDE_DIR})
	link_libraries(${GLUT_LIBRARIES})
endif()

find_package( OpenGL REQUIRED ) 
if (OPENGL_FOUND)
	include_directories(${OPENGL_INCLUDE_DIR})
	link_libraries(${OPENGL_LIBRARIES})
endif()

#include_directories()
#link_libraries()

include_directories( include . )
add_library( muskat muskat.cpp utils.cpp si.cpp main.cpp )

add_executable( Muskat main.cpp )
target_link_libraries( Muskat muskat )
