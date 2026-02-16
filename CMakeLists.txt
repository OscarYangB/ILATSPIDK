cmake_minimum_required(VERSION "4.2.1")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

add_subdirectory("external/entt")
add_subdirectory("external/SDL")

project("ILATSPIDK")
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")
add_executable(ILATSPIDK "${MY_SOURCES}")
target_link_libraries(ILATSPIDK PRIVATE EnTT::EnTT SDL3::SDL3-static)
