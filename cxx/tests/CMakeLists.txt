
add_executable(utils_test ${CMAKE_CURRENT_LIST_DIR}/utils-test.cc)
target_link_libraries(utils_test PRIVATE utils gtest::gtest)
if (WIN32)
	target_compile_options(utils_test PRIVATE /utf-8)
endif()

add_test(NAME utils_test COMMAND $<TARGET_FILE:utils_test>)
