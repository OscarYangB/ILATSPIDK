cmake_minimum_required(VERSION "4.2.1")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory("external/entt")
add_subdirectory("external/SDL")

option(FILE_EMBED "Embed files" OFF)
if(FILE_EMBED)
	add_compile_definitions(FILE_EMBED)
endif()

project("ILATSPIDK")
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")
add_executable(ILATSPIDK "${MY_SOURCES}")
target_link_libraries(ILATSPIDK PRIVATE EnTT::EnTT SDL3::SDL3-static)
