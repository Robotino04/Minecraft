cmake_minimum_required(VERSION 3.0)

include(FindPythonInterp)

add_custom_command(
    OUTPUT "${PROJECT_SOURCE_DIR}/Minecraft/src/include/Minecraft/PacketID.hpp"
    COMMAND ${PYTHON_EXECUTABLE} "${PROJECT_SOURCE_DIR}/Tools/PacketIDGrabber.py" "${PROJECT_SOURCE_DIR}/Minecraft/src/include/Minecraft/PacketID.tpp" "-q"
    MAIN_DEPENDENCY "${PROJECT_SOURCE_DIR}/Minecraft/src/include/Minecraft/PacketID.tpp"
    DEPENDS "${PROJECT_SOURCE_DIR}/Tools/PacketIDGrabber.py"
    VERBATIM
)

file(GLOB_RECURSE MINECRAFT_SRC "*.cpp" "*.hpp" "*.tpp")
list(APPEND MINECRAFT_SRC "${PROJECT_SOURCE_DIR}/Minecraft/src/include/Minecraft/PacketID.hpp")

add_library(minecraft STATIC ${MINECRAFT_SRC})
target_include_directories(minecraft PUBLIC "src/include/")
