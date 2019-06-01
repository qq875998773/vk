# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("VK")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(Vulkan REQUIRED)

find_package(GLFW3 REQUIRED)

# Include sub-projects.
add_subdirectory("shaders")
add_subdirectory("resources")

add_subdirectory ("app")
