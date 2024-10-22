enable_language(ASM_MASM)

set(LIB_NAME shell)

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
        ${HEADER_PATH}/install_shell.h
    PRIVATE
        install_shell.c
        entry_export.h
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "[!] Must configuring on/for x86 platform")
else()
    target_sources(${LIB_NAME}
        PRIVATE
            entry_x86.asm
    )

    set_source_files_properties(entry_x86.asm
        PROPERTIES
            COMPILE_FLAGS "/c /coff"
            LINKER_FLAGS "/SAFESEH:NO"
    )
endif()