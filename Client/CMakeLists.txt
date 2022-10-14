cmake_minimum_required(VERSION 3.0)

file(GLOB_RECURSE CLIENT_SRC "*.cpp" "*.hpp")

add_executable(client ${CLIENT_SRC})
target_include_directories(client PUBLIC "src/include/")

target_link_libraries(client minecraft)
