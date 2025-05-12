#include "gtest/gtest.h"
#include <sstream>
#include "../common/binaryio.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

// Test for writing binary data
TEST(BinaryIOTest, WriteBinaryInt) {
  std::ostringstream output;
  int const value = 42;
  write_binary(output, value);
  ASSERT_EQ(output.str().size(), sizeof(int));
}

// Test for reading binary data
TEST(BinaryIOTest, ReadBinaryInt) {
  int original_value = 42;
  std::string const buffer(reinterpret_cast<char*>(&original_value), sizeof(original_value));
  std::istringstream input(buffer);
  int const value = read_binary<int>(input);
  EXPECT_EQ(value, 42);
}

// Failure in read_binary
TEST(BinaryIOTest, ReadBinaryFailure) {
  std::istringstream input;
  input.setstate(std::ios::badbit);
  EXPECT_THROW(read_binary<int>(input), std::runtime_error);
}

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

