#ifndef COMPRESSAOS_HPP
#define COMPRESSAOS_HPP

#include "imageaos.hpp"
#include "../common/binaryio.hpp"
#include <vector>
#include <cstdint>


// Constants defining the maximum index sizes for 1, 2, and 4 bytes
const int MAX_INDEX_SIZE_1B = 256;
const int MAX_INDEX_SIZE_2B = 65536;
const long long MAX_INDEX_SIZE_4B = 4294967296;

// Writes the header of the compressed file, including basic image information
void write_header(std::ostream &output, const PPMImageAOS &image, size_t color_table_size);

// Helper function to write the color table to the output stream
template<typename PixelType>
void write_color_table(std::ostream &output, const std::vector <PixelType> &colors) {
    for (const auto &color: colors) {
        write_binary(output, color.red);
        write_binary(output, color.green);
        write_binary(output, color.blue);
    }
}

// Generates a color table and assigns unique indices to each color in the image
template<typename PixelType>
auto
generate_color_table(const std::vector <PixelType> &pixels, std::vector <PixelType> &unique_colors) -> std::map<PixelType, typename std::vector<PixelType>::size_type> {
    std::map<PixelType, typename std::vector<PixelType>::size_type> color_map;
    for (const auto &pixel: pixels) {
        if (color_map.find(pixel) == color_map.end()) { // Only add new colors
            color_map[pixel] = unique_colors.size();
            unique_colors.push_back(pixel);
        }
    }
    return color_map;
}

// Writes pixel indices to the output stream using the appropriate index type based on color map
template<typename PixelType, typename IndexType>
void write_pixel_indices(std::ostream &output, const std::vector <PixelType> &pixels,
                         const std::map<PixelType, typename std::vector<PixelType>::size_type> &color_map) {
    for (const auto &pixel: pixels) {
        auto index = static_cast<IndexType>(color_map.at(pixel)); // Convert index to specified type
        write_binary(output, index); // Write the index to the output
    }
}

// Processes an image with SmallPixel format, generates color table, and writes compressed data
void process_small_pixel_image(std::ostream &output, const PPMImageAOS &image);

// Processes an image with LargePixel format, generates color table, and writes compressed data
void process_large_pixel_image(std::ostream &output, const PPMImageAOS &image);

// Main function to compress the image and write it in a custom compressed format
void write_cppm(const std::string &output_file, const PPMImageAOS &image);


#endif //COMPRESSAOS_HPP
