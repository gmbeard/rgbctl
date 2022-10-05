set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Werror -Wextra -Wconversion -Wpedantic)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

#link_libraries(udev)
add_library(
    ${PROJECT_NAME}_library

    acquire.cpp
    assert.cpp
    builtin_modules.cpp
    builtins/asus/asus_x570.cpp
    builtins/corsair/corsair_h100i_pro_xt.cpp
    controller.cpp
    device_context.cpp
    effects.cpp
    effects/linear.cpp
    effects/rotate.cpp
    loop.cpp
    raw_device_stream.cpp
    rgb.cpp
    texture.cpp
    utils.cpp
)

set_target_properties(
    ${PROJECT_NAME}_library
    PROPERTIES
    ARCHIVE_OUTPUT_NAME ${PROJECT_NAME}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBRARY_OUTPUT_NAME ${PROJECT_NAME}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_executable(
    ${PROJECT_NAME}_prog
    detected_device.cpp
    detector.cpp
    main.cpp
)

target_link_libraries(
    ${PROJECT_NAME}_prog
    PRIVATE
    ${PROJECT_NAME}_library
    udev Tcc::Tcc
)

set_target_properties(
    ${PROJECT_NAME}_prog
    PROPERTIES
        RUNTIME_OUTPUT_NAME ${PROJECT_NAME}
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_executable(
    ${PROJECT_NAME}_detect
    devices.cpp
)

set_target_properties(
    ${PROJECT_NAME}_detect
    PROPERTIES
        RUNTIME_OUTPUT_NAME ${PROJECT_NAME}-detect
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_link_libraries(
    ${PROJECT_NAME}_detect
    PRIVATE
    udev
)

