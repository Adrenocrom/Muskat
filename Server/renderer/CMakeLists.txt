cmake_minimum_required( VERSION 2.8 )

project( muskat )

set( CMAKE_BUILD_TYPE "RELEASE" )
set( CMAKE_CXX_FLAGS "-Wall -O3 -std=c++11" )
add_definitions( ${CMAKE_CXX_FLAGS} )

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
        message(FATAL_ERROR "GCC version >=4.8 required.")
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.4)
        message(FATAL_ERROR "Clang version >=3.4 required.")
    endif()
else()
    message(WARNING "Your compiler (${CMAKE_CXX_COMPILER_ID}) has not been tested on this project. Only Clang and GCC has been tested. Please report any problems at the project page on GitHub.")
endif()

find_package( OpenMP )
if( OPENMP_FOUND) 
	set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" )
	set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
endif()

#Only tested with versions 1.55 and 1.56
find_package(Boost 1.54.0 COMPONENTS system regex coroutine context thread REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

find_package(Threads REQUIRED)

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

link_libraries(/usr/lib/x86_64-linux-gnu/libturbojpeg.a)

include_directories( include . )
add_library( muskat deamon.cpp muskat.cpp utils.cpp si.cpp scene.cpp main.cpp )

add_executable( Muskat main.cpp )
target_link_libraries( Muskat muskat turbojpeg)

target_link_libraries(Muskat ${Boost_LIBRARIES})
target_link_libraries(Muskat ${OPENSSL_CRYPTO_LIBRARY})
target_link_libraries(Muskat ${CMAKE_THREAD_LIBS_INIT})

if(MSYS)
    target_link_libraries(Muskat ws2_32 wsock32)
endif()
