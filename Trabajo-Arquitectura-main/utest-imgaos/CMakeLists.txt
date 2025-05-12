add_executable(utest-imgaos
        utest_compressaos.cpp
        utest_cutfreqaos.cpp
        utest_resizeaos.cpp
        utest_maxlevelaos.cpp
        utest_imageaos.cpp)

# Library dependencies
target_link_libraries (utest-imgaos PRIVATE common imgaos GTest::gtest_main Microsoft.GSL::GSL)


include(GoogleTest)
gtest_discover_tests(utest-imgaos)

# Register the test with CTest
add_test(NAME utest-imgaos COMMAND utest-imgaos)
