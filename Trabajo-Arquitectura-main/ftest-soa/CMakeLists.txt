add_executable(ftest-soa
        ftest_imtoolsoa.cpp
        )

# Library dependencies
target_link_libraries(ftest-soa PRIVATE imgsoa GTest::gtest_main Microsoft.GSL::GSL)

include(GoogleTest)
gtest_discover_tests(ftest-soa)

# Register the test with CTest
add_test(NAME ftest-soa COMMAND ftest-soa)
