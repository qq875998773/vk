# Add source to this project's executable.
set(APP_SRC
    "main.cpp"
    "application.cpp"
    "application.hpp"
    "vk_render.cpp"
    "vk_render.hpp"
)

add_executable (vk ${APP_SRC})

target_include_directories(vk PUBLIC "include")
target_link_libraries(vk PRIVATE Vulkan::Vulkan glfw)

add_dependencies(vk Shaders Resources)

if (WIN32)
    add_custom_command(TARGET vk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${GLFW3_DLL}
            $<TARGET_FILE_DIR:vk>
    )
endif ()