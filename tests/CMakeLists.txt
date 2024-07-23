
include(CTest)

set(UNITY_VERSION v2.6.0)
find_package(unity ${unity_FOUND} QUIET)
if (NOT unity_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        unity
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/ThrowTheSwitch/Unity/archive/refs/tags/${UNITY_VERSION}.tar.gz
    )
    FetchContent_MakeAvailable(unity)
endif()

file(GLOB_RECURSE SOURCES "*.c")

target_link_libraries(${PROJECT_NAME} unity)
