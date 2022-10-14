cmake_minimum_required(VERSION 3.0)

file(GLOB_RECURSE SERVER_SRC "*.cpp" "*.hpp")

add_executable(server ${SERVER_SRC})
target_include_directories(client PUBLIC "src/include/")

target_link_libraries(server minecraft)