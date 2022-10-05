set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Werror -Wextra -Wconversion -Wpedantic)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

add_library(
    testing

    testing.cpp
)

include_directories(${CMAKE_CURRENT_LIST_DIR})
link_libraries(dl testing Tcc::Tcc ${PROJECT_NAME}_library)

add_executable(module_tests module_tests.cpp)
add_test(NAME module_tests COMMAND module_tests)

add_executable(product_id_tests product_id_tests.cpp)
add_test(NAME product_id_tests COMMAND product_id_tests)

add_executable(rgb_tests rgb_tests.cpp)
add_test(NAME rgb_tests COMMAND rgb_tests)

add_executable(effect_tests effect_tests.cpp)
add_test(NAME effect_tests COMMAND effect_tests)

add_executable(scaling_tests scaling_tests.cpp)
add_test(NAME scaling_tests COMMAND scaling_tests)

add_executable(shader_tests shader_tests.cpp)
add_test(NAME shader_tests COMMAND shader_tests)

add_executable(driver_plugin_tests driver_plugin_tests.cpp)
add_test(NAME driver_plugin_tests COMMAND driver_plugin_tests)

add_executable(dynamic_module_tests dynamic_module_tests.cpp)
add_test(
    NAME dynamic_module_tests
    COMMAND dynamic_module_tests "${CMAKE_CURRENT_LIST_DIR}/dynamic_module.c"
)
