cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(opengl-template-project)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/externals/arl/include)
include_directories("${EIGEN3_INCLUDE_DIR}")


# link_directories(${CMAKE_CURRENT_SOURCE_DIR}/externals/arl/build/source/utilities/csv)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings -Wno-deprecated \
    -Wall -Wextra -pedantic -fdiagnostics-color")
set (CMAKE_SKIP_RULE_DEPENDENCY TRUE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage -std=c++11")
endif()

add_executable (opengl-template-project-main main.cpp
                                        visualizer/visualizer.cpp
                                        visualizer/view_angles.cpp
                                        visualizer/axes_box.cpp
                                        visualizer/mouse_state.cpp)
target_link_libraries (opengl-template-project-main ${OPENGL_LIBRARIES}
                                                    ${GLUT_LIBRARY}
                                                    ${GLEW_LIBRARIES}
                                                    ${OpenCV_LIBS}
                                                    ${NANO_GUI_LIB})
