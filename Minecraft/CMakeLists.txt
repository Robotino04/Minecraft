cmake_minimum_required(VERSION 3.0)

file(GLOB_RECURSE MINECRAFT_SRC "*.cpp" "*.hpp")

add_library(minecraft STATIC ${MINECRAFT_SRC})
target_include_directories(minecraft PUBLIC "src/include/")
