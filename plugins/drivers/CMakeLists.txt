add_library(
    testdriver
    SHARED
    test_driver.c
)

target_link_libraries(
    testdriver
    PRIVATE
    ${PROJECT_NAME}_library
)
