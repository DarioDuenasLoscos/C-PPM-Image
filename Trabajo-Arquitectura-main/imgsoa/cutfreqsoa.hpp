#ifndef CUTFREQSOA_HPP
#define CUTFREQSOA_HPP

#include "imagesoa.hpp"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <cmath>
#include <algorithm>

const int BITS_FOR_1B = 8;

template<typename ComponentType, typename ColorCodeType>
void
sortColors(std::vector <ColorCodeType> &color_list,
           const std::unordered_map <ColorCodeType, size_t> &color_frequencies) {
    std::sort(color_list.begin(), color_list.end(),
              [&color_frequencies](ColorCodeType color_a, ColorCodeType color_b) {
                  size_t const frequency_a = color_frequencies.at(color_a);
                  size_t const frequency_b = color_frequencies.at(color_b); // Frequency of colors a and b
                  if (frequency_a != frequency_b) { return frequency_a < frequency_b;} // Compare by frequency

                  auto blue_a = (color_a) &
                                ((1ULL << (sizeof(ComponentType) * BITS_FOR_1B)) - 1); // Extract blue component of a
                  auto blue_b = (color_b) &
                                ((1ULL << (sizeof(ComponentType) * BITS_FOR_1B)) - 1); // Extract blue component of b
                  if (blue_a != blue_b) { return blue_a > blue_b;}// Compare by blue component

                  auto green_a = (color_a >> (sizeof(ComponentType) * BITS_FOR_1B)) &
                                 ((1ULL << (sizeof(ComponentType) * BITS_FOR_1B)) - 1); // Extract green component of a
                  auto green_b = (color_b >> (sizeof(ComponentType) * BITS_FOR_1B)) &
                                 ((1ULL << (sizeof(ComponentType) * BITS_FOR_1B)) - 1); // Extract green component of b
                  if (green_a != green_b) { return green_a > green_b;} // Compare by green component

                  auto red_a = (color_a >> (sizeof(ComponentType) * BITS_FOR_1B * 2)) &
                               ((1ULL << (sizeof(ComponentType) * BITS_FOR_1B)) - 1); // Extract red component of a
                  auto red_b = (color_b >> (sizeof(ComponentType) * BITS_FOR_1B * 2)) &
                               ((1ULL << (sizeof(ComponentType) * BITS_FOR_1B)) - 1); // Extract red component of b
                  return red_a > red_b; // Compare by red component
              });
}

template<typename ComponentType, typename ColorCodeType>
void FindNearestColors(const std::unordered_set <ColorCodeType> &colors_to_remove,
                       std::unordered_map <ColorCodeType, ColorCodeType> &replacement_map,
                       const std::unordered_set <ColorCodeType> &colors_to_keep) {
    for (auto color_remove: colors_to_remove) {
        ComponentType red_remove = (color_remove >> (sizeof(ComponentType) * BITS_FOR_1B * 2)) &
                                   ((1ULL << (sizeof(ComponentType) * BITS_FOR_1B)) -
                                    1); // Extract red component of color to remove
        ComponentType green_remove = (color_remove >> (sizeof(ComponentType) * BITS_FOR_1B)) &
                                     ((1ULL << (sizeof(ComponentType) * BITS_FOR_1B)) -
                                      1); // Extract green component of color to remove
        ComponentType blue_remove = color_remove & ((1ULL << (sizeof(ComponentType) * BITS_FOR_1B)) -
                                                    1); // Extract blue component of color to remove

        double min_distance = std::numeric_limits<double>::max(); // Initialize minimum distance with a large value
        ColorCodeType closest_color = color_remove; // Initialize the closest color with current color

        for (auto color_keep: colors_to_keep) {
            ComponentType red_keep = (color_keep >> (sizeof(ComponentType) * BITS_FOR_1B * 2)) &
                                     ((1ULL << (sizeof(ComponentType) * BITS_FOR_1B)) -
                                      1); // Extract red component of color to keep
            ComponentType green_keep = (color_keep >> (sizeof(ComponentType) * BITS_FOR_1B)) &
                                       ((1ULL << (sizeof(ComponentType) * BITS_FOR_1B)) -
                                        1); // Extract green component of color to keep
            ComponentType blue_keep = color_keep & ((1ULL << (sizeof(ComponentType) * BITS_FOR_1B)) -
                                                    1); // Extract blue component of color to keep

            double const distance = std::pow(red_remove - red_keep, 2) + std::pow(green_remove - green_keep, 2) +
                              std::pow(blue_remove - blue_keep, 2); // Calculate squared Euclidean distance

            if (distance < min_distance) { // Update minimum distance and closest color if a closer one is found
                min_distance = distance;
                closest_color = color_keep;
            }
        }
        replacement_map[color_remove] = closest_color; // Map the color to be removed to its closest color
    }
}

template<typename ComponentType, typename ColorCodeType>
void removeColors(int num_colors_to_remove,
                  std::vector <ComponentType> &red,
                  std::vector <ComponentType> &green,
                  std::vector <ComponentType> &blue) {
    if (static_cast<size_t>(num_colors_to_remove) > red.size()) {
        // If n is greater than or equal to the total pixel count, set all pixels to black
        std::fill(red.begin(), red.end(), 0);
        std::fill(green.begin(), green.end(), 0);
        std::fill(blue.begin(), blue.end(), 0);
        return;}
    size_t const pixel_count = red.size();
    std::unordered_map <ColorCodeType, size_t> color_frequencies;
    std::vector <ColorCodeType> color_list;

    for (size_t index = 0; index < pixel_count; ++index) {
        ColorCodeType color_code = ((ColorCodeType) red[index] << (sizeof(ComponentType) * BITS_FOR_1B * 2)) |
                                   ((ColorCodeType) green[index] << (sizeof(ComponentType) * BITS_FOR_1B)) |
                                   blue[index]; // Pack RGB components into a color code
        if (color_frequencies[color_code]++ == 0) {
            color_list.push_back(color_code);} // Store unique colors
    }

    sortColors<ComponentType, ColorCodeType>(color_list, color_frequencies); // Sort colors by frequency and components
    std::unordered_set <ColorCodeType> const colors_to_remove(color_list.begin(), color_list.begin() + std::min(num_colors_to_remove, (int) color_list.size())); // Colors to remove
    std::unordered_set <ColorCodeType> const colors_to_keep(color_list.begin() + std::min(num_colors_to_remove, (int) color_list.size()), color_list.end()); // Colors to keep
    std::unordered_map <ColorCodeType, ColorCodeType> replacement_map;
    FindNearestColors<ComponentType, ColorCodeType>(colors_to_remove, replacement_map, colors_to_keep); // Create replacement map
    for (size_t index = 0; index < pixel_count; ++index) {
        ColorCodeType color_code = ((ColorCodeType) red[index] << (sizeof(ComponentType) * BITS_FOR_1B * 2)) |
                                   ((ColorCodeType) green[index] << (sizeof(ComponentType) * BITS_FOR_1B)) |
                                   blue[index]; // Pack RGB components into a color code
        if (colors_to_remove.contains(color_code)) { // Check if color is in the remove set
            ColorCodeType replacement_color = replacement_map[color_code];
            red[index] = (replacement_color >> (sizeof(ComponentType) * BITS_FOR_1B * 2)) &
                         ((1ULL << (sizeof(ComponentType) * BITS_FOR_1B)) - 1); // Replace red component
            green[index] = (replacement_color >> (sizeof(ComponentType) * BITS_FOR_1B)) &
                           ((1ULL << (sizeof(ComponentType) * BITS_FOR_1B)) - 1); // Replace green component
            blue[index] =
                    replacement_color & ((1ULL << (sizeof(ComponentType) * BITS_FOR_1B)) - 1); // Replace blue component
        }
    }
}

void removeLeastFrequentColors(SOAImage &image, int num_colors_to_remove);

#endif // CUTFREQSOA_HPP
