# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("VK")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(FindPackageHandleStandardArgs)

if (WIN32)
    find_package(GLFW3 REQUIRED)
elseif (UNIX AND NOT APPLE)
    find_package(glfw3 REQUIRED)
endif ()

find_package(Vulkan REQUIRED)

# Include sub-projects.
add_subdirectory("shaders")
add_subdirectory("resources")

add_subdirectory ("app")
