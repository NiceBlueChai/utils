

add_executable(utils_test ${CMAKE_CURRENT_LIST_DIR}/utils-test.cc)
target_link_libraries(utils_test  utils gtest::gtest)
if (WIN32)
	target_compile_options(utils_test PRIVATE /utf-8)
endif()
