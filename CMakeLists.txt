cmake_minimum_required(VERSION 3.25.1)
project(imtool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Werror -pedantic -pedantic-errors -Wconversion -Wsign-conversion)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")

# Support FetchContent functionality
include(FetchContent)
# Unit tests with google test
#Fetch googletest
FetchContent_Declare(googletest
        GIT_REPOSITORY "https://github.com/google/googletest"
        GIT_TAG "v1.15.2"
        GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(googletest)
# Enable GSL Library
FetchContent_Declare(GSL
        GIT_REPOSITORY "https://github.com/microsoft/GSL"
        GIT_TAG "v4.0.0"
        GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(GSL)

# Enable clang-tidy
set(CMAKE_CXX_CLANG_TIDY clang-tidy-19;
        "-format-style=file"
        "-header-filter=.*")

# Add include directory
include_directories(PUBLIC .)

# Add library subdirectory
add_subdirectory(common)
add_subdirectory(imgaos)
add_subdirectory(imgsoa)

# Add executable
add_subdirectory(imtool-aos)
add_subdirectory(imtool-soa)

# Add Unit tests and functional tests
enable_testing()
add_subdirectory(utest-imgaos)
add_subdirectory(utest-imgsoa)
add_subdirectory(utest-common)
add_subdirectory(ftest-aos)
add_subdirectory(ftest-soa)

