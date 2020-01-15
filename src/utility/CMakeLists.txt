add_library(utility "")

set(HEADER_PATH "${CMAKE_SOURCE_DIR}/include/utility")

target_include_directories(utility PUBLIC ${HEADER_PATH})

target_sources(utility
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/error_handling.c
        ${CMAKE_CURRENT_LIST_DIR}/file_access.c
    PUBLIC
        ${HEADER_PATH}/error_handling.h
        ${HEADER_PATH}/file_access.h
)