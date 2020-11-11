cmake_minimum_required(VERSION 3.17)
project(SpaceInvaders C)

set(CMAKE_C_STANDARD 99)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # or games

add_subdirectory(libs/raylib)

add_executable(SpaceInvaders main.c)

target_link_libraries(SpaceInvaders PRIVATE raylib)