project(glad)

add_library(glad STATIC src/gl.c src/wgl.c)

target_include_directories(glad PUBLIC include/)