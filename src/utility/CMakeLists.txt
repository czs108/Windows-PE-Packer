set(LIB_NAME utility)

add_library(${LIB_NAME})

target_compile_definitions(${LIB_NAME}
    PRIVATE
        "-D_UNICODE"
        "-DUNICODE"
)

set(HEADER_PATH ${PROJECT_SOURCE_DIR}/include/${LIB_NAME})

target_include_directories(${LIB_NAME}
    PUBLIC
        ${HEADER_PATH}
)

target_sources(${LIB_NAME}
    PUBLIC
        ${HEADER_PATH}/error_handling.h
        ${HEADER_PATH}/file_access.h
        ${HEADER_PATH}/platform_check.h
        ${HEADER_PATH}/encrypt.h
    PRIVATE
        error_handling.c
        file_access.c
        encrypt.c
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_sources(${LIB_NAME}
        PRIVATE
            platform_check_x64.c
    )
else()
    target_sources(${LIB_NAME}
        PRIVATE
            platform_check_x86.c
    )
endif()