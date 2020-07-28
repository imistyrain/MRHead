cmake_minimum_required(VERSION 2.8)

option(PROJECT_NAME "project name" glfwtest)

project(${PROJECT_NAME})
option(BUILD_CV "build with opencv" ON)
set(CMAKE_CXX_STANDARD 11)
include_directories(./)
add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cpp glad.c)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${GLEW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} glfw GLEW::GLEW)

if(OPENCV_FOUND)
    target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
endif()

file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})