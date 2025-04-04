cmake_minimum_required(VERSION 3.14)
project(FalconGL)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox /fp:fast /GL /DNDEBUG")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
endif()

set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(PROJECT_ROOT="${PROJECT_SOURCE_DIR}")
# add_definitions(-DPROJECT_ROOT="${PROJECT_SOURCE_DIR}")

find_package(glm CONFIG REQUIRED)

add_executable(FalconGLTest main.cpp)

target_link_libraries(FalconGLTest PRIVATE glm::glm)
