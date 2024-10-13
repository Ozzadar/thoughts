cmake_minimum_required(VERSION 3.27)
project(thoughts)

set(CMAKE_CXX_STANDARD 20)
option(WIL_BUILD_TESTS "Build the WIL tests" OFF)
option(WIL_BUILD_PACKAGING "Build the WIL packaging" OFF)

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
    target_link_libraries(${PROJECT_NAME} WIL)
endif()

target_link_libraries(${PROJECT_NAME} imguilib glfw glad spdlog::spdlog glm)