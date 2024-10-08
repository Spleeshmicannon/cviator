cmake_minimum_required(VERSION 3.22)
Project(_)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_COMPILER clang)

set(CMAKE_C_ARGS "${CMAKE_C_ARGS} -02 -0fast -fstrict-aliasing")

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
set(GLEW_USE_STATIC_LIBS ON)
find_package(GLEW REQUIRED)
find_package(cglm REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(cviator src/main.c)
# the m at the end is for OpenMPI (used by cglm)
target_link_libraries(cviator PRIVATE ${SDL2_LIBRARIES} OpenGL::OpenGL GLEW::GLEW cglm::cglm m)
target_include_directories(cviator PUBLIC 3rdparty)
target_compile_features(cviator PUBLIC c_std_23)
target_compile_definitions(cviator PRIVATE CV_DEBUG GLEW_STATIC)
