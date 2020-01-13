add_executable(PE-Packer "")

set(HEADER_PATH "${CMAKE_SOURCE_DIR}/include")

target_include_directories(PE-Packer PRIVATE ${HEADER_PATH})

target_sources(PE-Packer
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/main.c
)