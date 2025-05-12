#ifndef BINARY_IO_HPP
#define BINARY_IO_HPP

#include <stdexcept>
#include <iostream>

template<typename T>
auto read_binary(std::istream &input) -> T {
    T value;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    if (!input.read(reinterpret_cast<char*>(&value), sizeof(value))) {
      throw std::runtime_error("Error reading binary data");
    }
    return value;
}

template<typename T>
void write_binary(std::ostream &output, T const &value) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    if (!output.write(reinterpret_cast<const char*>(&value), sizeof(value))) {
      throw std::runtime_error("Failed to write binary data.");
    }
}

#endif // BINARY_IO_HPP
