set(RESOURCE_DIRS
    "models"
    "textures"
)

foreach(RESOURCE_DIR ${RESOURCE_DIRS})
    set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE_DIR})
    set(DST ${PROJECT_BINARY_DIR}/resources/${RESOURCE_DIR})
    if (WIN32)
        FILE(TO_NATIVE_PATH ${SRC} SRC_WIN)
        FILE(TO_NATIVE_PATH ${DST} DST_WIN)
        set(DIR ${DST})
        add_custom_command(
            OUTPUT ${DIR}
            COMMAND IF NOT EXIST ${DST_WIN} mklink /J ${DST_WIN} ${SRC_WIN}
            DEPENDS ${SRC}
        )
    elseif (UNIX)
        add_custom_command(
            OUTPUT ${DIR}
            COMMAND ${CMAKE_COMMAND} -E create_symlink ${SRC} ${DST}
            DEPENDS ${SRC}
        )
    endif (WIN32)
    list(APPEND RESOURCES ${DIR})
endforeach(RESOURCE_DIR)

add_custom_target(
    Resources
    DEPENDS ${RESOURCES}
)
