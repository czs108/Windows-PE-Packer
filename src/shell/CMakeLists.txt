enable_language(ASM_MASM)

add_library(shell "")

set(HEADER_PATH "${CMAKE_SOURCE_DIR}/include/shell")

target_include_directories(shell PUBLIC ${HEADER_PATH})

target_sources(shell
    PRIVATE
    PUBLIC
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)

else()

endif()