
add_library(lsp STATIC lsp.c lsp.h message_queue.c message_queue.h)

set(YYJSON_VERSION 0.10.0)
find_package(yyjson ${YYJSON_VERSION} QUIET)
if (NOT yyjson_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        yyjson
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/ibireme/yyjson/archive/refs/tags/${YYJSON_VERSION}.tar.gz
    )

    FetchContent_MakeAvailable(yyjson)
endif()

target_link_libraries(lsp PRIVATE yyjson)
