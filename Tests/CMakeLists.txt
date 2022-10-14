cmake_minimum_required(VERSION 3.0)

file(GLOB_RECURSE TEST_SRC "*.cpp")

foreach(TEST_FILE ${TEST_SRC})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WLE)

    add_executable("test_${TEST_NAME}" "${TEST_FILE}")
    target_link_libraries("test_${TEST_NAME}" PUBLIC minecraft)
    add_test(${TEST_NAME} "test_${TEST_NAME}")
endforeach(TEST_FILE)
