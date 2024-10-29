cmake_minimum_required(VERSION 3.27)
project(thoughts)

set(CMAKE_CXX_STANDARD 20)
option(WIL_BUILD_TESTS "Build the WIL tests" OFF)
option(WIL_BUILD_PACKAGING "Build the WIL packaging" OFF)

# Initial musings were with GLFW, but I wanted to try native window hooks for some more fancy things I want to do
# this may or may not be a good idea, but it's a fun experiment
option(OZZ_WIN_NATIVE "Build using native window hooks. Otherwise use GLFW" OFF)
if (OZZ_WIN_NATIVE)
    add_compile_definitions(OZZ_WIN_NATIVE)
endif ()

# output to dist/platform/configuration directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE})

add_subdirectory(vendor/glad)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/wil)
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glm)

file (GLOB_RECURSE SOURCES src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include/)

# if on windows, link against wil
if (WIN32)
    add_compile_definitions(NOMINMAX)
    add_compile_definitions(GLFW_EXPOSE_NATIVE_WIN32)
    target_link_libraries(${PROJECT_NAME} WIL OpenGL32)
endif()

target_link_libraries(${PROJECT_NAME} imguilib glfw glad spdlog::spdlog glm)