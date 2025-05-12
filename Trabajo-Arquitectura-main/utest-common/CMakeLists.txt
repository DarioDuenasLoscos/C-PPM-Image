add_executable(utest-common
        utest_binaryio.cpp
        utest_progargs.cpp)

# Library dependencies
target_link_libraries(utest-common PRIVATE common GTest::gtest_main Microsoft.GSL::GSL)


include(GoogleTest)
gtest_discover_tests(utest-common)

# Register the test with CTest
add_test(NAME utest-common COMMAND utest-common)
