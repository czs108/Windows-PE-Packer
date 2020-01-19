add_subdirectory(utility)

add_executable(PE-Packer "")

set(HEADER_PATH "${CMAKE_SOURCE_DIR}/include")

target_include_directories(PE-Packer PRIVATE ${HEADER_PATH})

target_sources(PE-Packer
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/main.c

        ${CMAKE_CURRENT_LIST_DIR}/extra_data.c
        ${HEADER_PATH}/extra_data.h

        ${CMAKE_CURRENT_LIST_DIR}/image.c
        ${HEADER_PATH}/image.h

        ${CMAKE_CURRENT_LIST_DIR}/import_table.c
        ${HEADER_PATH}/import_table.h

        ${CMAKE_CURRENT_LIST_DIR}/section.c
        ${HEADER_PATH}/section.h
)

target_link_libraries(PE-Packer utility)