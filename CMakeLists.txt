cmake_minimum_required(VERSION 3.10)

project(Cube)

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} glfw GLEW GL X11 GLU OpenGL)
