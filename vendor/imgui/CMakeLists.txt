project(imguilib)

file(GLOB SRC_FILES
    imguilib/*.cpp
    imguilib/backends/imgui_impl_opengl3.cpp
    imguilib/backends/imgui_impl_glfw.cpp
)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ./imguilib ./imguilib/backends)

target_link_libraries(${PROJECT_NAME} PUBLIC glad glfw)
