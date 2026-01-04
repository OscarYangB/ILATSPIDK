cmake_minimum_required(VERSION "4.2.1")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

add_subdirectory("external/entt")
add_subdirectory("external/SDL")

project("ILATSPIDN")
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(my_program "${MY_SOURCES}")
target_link_libraries(my_program PRIVATE EnTT::EnTT SDL3::SDL3)