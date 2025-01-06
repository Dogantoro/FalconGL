cmake_minimum_required(VERSION 3.14)
project(DoganGL)

set(CMAKE_CXX_STANDARD 17)

find_package(glm CONFIG REQUIRED)

add_executable(DoganGLTest main.cpp)

target_link_libraries(DoganGLTest PRIVATE glm::glm)
