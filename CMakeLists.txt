cmake_minimum_required(VERSION 3.22)

project(samplelib)

# Remove CMake warning
set(DUMMY ${IN_DOCKER})

if (DEFINED IN_DOCKER)
    message("-- Build env: Dockerfile")
else()
    message("-- Build env: Host machine")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -Wall")
endif()

if (NOT DEFINED IN_DOCKER)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.11.0)
    FetchContent_MakeAvailable(googletest)

    FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.6.1)
    FetchContent_MakeAvailable(benchmark)
endif()

if (DEFINED HEADER_ONLY)
    add_library(${PROJECT_NAME} INTERFACE)
    target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_SOURCE_DIR}/include)
    if (WIN32)
        add_library(${PROJECT_NAME}_
            ${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}/sample.h)
        set_target_properties(${PROJECT_NAME}_ PROPERTIES LINKER_LANGUAGE CXX)
        target_link_libraries(${PROJECT_NAME}_ ${PROJECT_NAME})
        set_property(TARGET ${PROJECT_NAME}_ PROPERTY CXX_STANDARD 20)
    endif()
else()
    add_library(${PROJECT_NAME}
        src/${PROJECT_NAME}/sample.cpp)
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
endif()
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

add_executable(${PROJECT_NAME}_tests
    tests/sample.cpp
    tests/main.cpp)
target_include_directories(${PROJECT_NAME}_tests PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}_tests ${PROJECT_NAME} GTest::gtest GTest::gtest_main)
set_property(TARGET ${PROJECT_NAME}_tests PROPERTY CXX_STANDARD 20)

if (DEFINED IN_DOCKER)
    find_package(GTest REQUIRED)
    find_package(benchmark REQUIRED)
endif()

add_executable(${PROJECT_NAME}_benchmark
    benchmark/sample.cpp
    benchmark/main.cpp)
target_include_directories(${PROJECT_NAME}_benchmark PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}_benchmark ${PROJECT_NAME} benchmark::benchmark)
set_property(TARGET ${PROJECT_NAME}_benchmark PROPERTY CXX_STANDARD 20)
