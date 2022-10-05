cmake_minimum_required(VERSION 3.22)
project(
    rgbctl
    LANGUAGES CXX C
    VERSION 0.1.0
)

option(
    RGBCTL_ENABLE_TESTS
    "Enable tests for ${PROJECT_NAME}"
    OFF
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(Tcc REQUIRED)

include_directories(
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_subdirectory(src)
add_subdirectory(plugins)

if(RGBCTL_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
