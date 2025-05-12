add_executable(ftest-aos
        ftest_imtoolaos.cpp
        )

# Library dependencies
target_link_libraries(ftest-aos PRIVATE imgaos GTest::gtest_main Microsoft.GSL::GSL)


include(GoogleTest)
gtest_discover_tests(ftest-aos)

# Register the test with CTest
add_test(NAME ftest-aos COMMAND ftest-aos)
