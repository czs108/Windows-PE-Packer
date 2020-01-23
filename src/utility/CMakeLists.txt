add_library(utility "")

target_compile_definitions(utility PRIVATE -D_UNICODE -DUNICODE)

set(HEADER_PATH "${CMAKE_SOURCE_DIR}/include/utility")

target_include_directories(utility PUBLIC ${HEADER_PATH})

target_sources(utility
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/error_handling.c
        ${CMAKE_CURRENT_LIST_DIR}/file_access.c
        ${CMAKE_CURRENT_LIST_DIR}/encrypt.c
    PUBLIC
        ${HEADER_PATH}/error_handling.h
        ${HEADER_PATH}/file_access.h
        ${HEADER_PATH}/platform_check.h
        ${HEADER_PATH}/encrypt.h
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_sources(utility
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/platform_check_x64.c
    )
else()
    target_sources(utility
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/platform_check_x86.c
    )
endif()