cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 11)

project(DioMetric_Engine)

add_executable(main src/main.c)

# Uncomment these lines to setup SDL
# in case you dont have it installed system-wide

#include_directories(SDL include path)
#link_directories(SDL lib path)

target_link_libraries(main m)
target_link_libraries(main SDL2)
target_link_libraries(main SDL2_image)
target_link_libraries(main SDL2_ttf)
