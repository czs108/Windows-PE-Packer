add_executable(${CMAKE_PROJECT_NAME})

target_compile_definitions(${CMAKE_PROJECT_NAME}
    PRIVATE
        "-D_UNICODE"
        "-DUNICODE"
)

set(HEADER_PATH ${PROJECT_SOURCE_DIR}/include)

target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE
        ${HEADER_PATH}
)

target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        main.c
        extra_data.c
        image.c
        import_table.c
        section.c
        ${HEADER_PATH}/extra_data.h
        ${HEADER_PATH}/image.h
        ${HEADER_PATH}/import_table.h
        ${HEADER_PATH}/section.h
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
        utility
        shell
)

add_subdirectory(utility)
add_subdirectory(shell)