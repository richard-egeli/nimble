
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

function(test TEST_NAME)
    set(KEYWORD "SOURCES")
    set(location TRUE)
    set(libraries "")
    set(sources "")

    foreach(arg ${ARGN})
        if(arg STREQUAL "SOURCES")
            set(location "SOURCES")
        elseif(arg STREQUAL "LIBRARIES")
            set(location "LIBRARIES")
        else()
            if(location STREQUAL "SOURCES")
                list(APPEND sources ${arg})
            elseif(location STREQUAL "LIBRARIES")
                list(APPEND libraries ${arg})
            endif()
        endif()
    endforeach()

    add_executable(${TEST_NAME} ${sources})
    if(libraries)
        target_link_libraries(${TEST_NAME} ${libraries} unity)
    endif()

    add_test(${TEST_NAME}_suite ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_NAME})
endfunction()

test(test_string SOURCES test_string.c LIBRARIES text)
test(test_string_array SOURCES test_string_array.c LIBRARIES text)
