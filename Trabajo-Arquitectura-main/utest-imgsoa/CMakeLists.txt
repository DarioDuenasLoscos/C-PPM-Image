add_executable(utest-imgsoa
        utest_compresssoa.cpp
        utest_cutfreqsoa.cpp
        utest_resizesoa.cpp
        utest_maxlevelsoa.cpp
        utest_imagesoa.cpp)

target_link_libraries(utest-imgsoa PRIVATE common imgsoa GTest::gtest_main Microsoft.GSL::GSL)

include(GoogleTest)
gtest_discover_tests(utest-imgsoa)

# Register the test with CTest
add_test(NAME utest-imgsoa COMMAND utest-imgsoa)
