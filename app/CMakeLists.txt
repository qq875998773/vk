# CMakeList.txt : CMake project for vk, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
set(APP_SRC
    "app.h"
    "app.cpp"
)

add_executable (vk ${APP_SRC})

target_include_directories(vk PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(vk ${Vulkan_LIBRARIES} glfw3::glfw3)

add_custom_command(TARGET vk POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${GLFW3_DLL}
        $<TARGET_FILE_DIR:vk>
)
