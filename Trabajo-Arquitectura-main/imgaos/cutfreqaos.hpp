#ifndef CUTFREQAOS_HPP
#define CUTFREQAOS_HPP

#include "imageaos.hpp"
#include <cstddef>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>
#include <functional>




// Define constants
constexpr int KDTREE_DIMENSION = 3;
constexpr int HASH_VALUE_1 = 8;
constexpr int HASH_VALUE_2 = 16;
constexpr int HASH_VALUE_3 = 32;

template<typename PixelType>
struct KDTreeSearchContext {
    std::vector<PixelType> tree;
    PixelType target;
    double min_dist;
    [[maybe_unused]] PixelType nearest;

};

// Specializations of std::hash for SmallPixel and LargePixel
namespace std {
  template<>
  struct [[maybe_unused]] hash<SmallPixel> {
      auto operator()(const SmallPixel &pixel) const -> size_t {
        return (static_cast<size_t>(pixel.red) << HASH_VALUE_2) | (static_cast<size_t>(pixel.green) << HASH_VALUE_1) | pixel.blue;
      }
  };

  template<>
  struct [[maybe_unused]] hash<LargePixel> {
      auto operator()(const LargePixel &pixel) const -> size_t {
        return (static_cast<size_t>(pixel.red) << HASH_VALUE_3) | (static_cast<size_t>(pixel.green) << HASH_VALUE_2) | pixel.blue;
      }
  };
}

// Function to build the k-d tree recursively
// NOLINTBEGIN(misc-no-recursion)
template<typename PixelType>
void buildKdTree(std::vector<PixelType> &points, size_t left, size_t right, int depth) {
  if (left >= right) { return;}
    int const axis = depth % KDTREE_DIMENSION;
  size_t mid = left + ((right - left) / 2);

  // Partial sort based on the axis
  std::nth_element(points.begin() + static_cast<typename std::vector<PixelType>::difference_type>(left),
                   points.begin() + static_cast<typename std::vector<PixelType>::difference_type>(mid),
                   points.begin() + static_cast<typename std::vector<PixelType>::difference_type>(right),
                   [axis](const PixelType &pix_a, const PixelType &pix_b) {
                     if (axis == 0) { return pix_a.red < pix_b.red;}
                         if (axis == 1) { return pix_a.green < pix_b.green;}
                         return pix_a.blue < pix_b.blue;
                   });

  // Recursively build left and right subtrees
  buildKdTree(points, left, mid, depth + 1);
  buildKdTree(points, mid + 1, right, depth + 1);
}

// Helper function to compute squared distance
template<typename PixelType>
auto squaredDistance(const PixelType &pix_a, const PixelType &pix_b) -> double {
  double distr = static_cast<double>(pix_a.red) - static_cast<double>(pix_b.red);
  double distg = static_cast<double>(pix_a.green) - static_cast<double>(pix_b.green);
  double distb = static_cast<double>(pix_a.blue) - static_cast<double>(pix_b.blue);
  return (distr * distr) + (distg * distg) + (distb * distb);
}

// Function to search nearest neighbor recursively
template<typename PixelType>
void kdTreeNearestNeighbor(KDTreeSearchContext<PixelType> &context, size_t left, size_t right, int depth) {
  if (left >= right) { return; }

  int const axis = depth % KDTREE_DIMENSION;
  size_t mid = left + ((right - left) / 2);

  const PixelType &pivot = context.tree[mid];
  double dist = squaredDistance(context.target, pivot);
  if (dist < context.min_dist) {
    context.min_dist = dist;
    context.nearest = pivot;
  }

  double diff = 0.0;
  if (axis == 0) { diff = static_cast<double>(context.target.red) - static_cast<double>(pivot.red);
  } else if (axis == 1) { diff = static_cast<double>(context.target.green) - static_cast<double>(pivot.green);
  } else { diff = static_cast<double>(context.target.blue) - static_cast<double>(pivot.blue);
  }

  if (diff <= 0) {
    // first left subtree
    kdTreeNearestNeighbor(context, left, mid, depth + 1);
    if (diff * diff < context.min_dist) {
      // right subtree if neccessary
      kdTreeNearestNeighbor(context, mid + 1, right, depth + 1);
    }
  } else {
    // right subtree
    kdTreeNearestNeighbor(context, mid + 1, right, depth + 1);
    if (diff * diff < context.min_dist) {
      // left subtree if neccessary
      kdTreeNearestNeighbor(context, left, mid, depth + 1);
    }
  }
}
// NOLINTEND(misc-no-recursion)
// Function to find nearest colors using k-d tree
template<typename PixelType>
void FindNearestColors(const std::unordered_set<PixelType> &colors_to_remove,
                       std::unordered_map<PixelType, PixelType> &replacement_map,
                       std::vector<PixelType> &colors_to_keep_vec) {
  // Build k-d tree from colors_to_keep
  buildKdTree<PixelType>(colors_to_keep_vec, 0, colors_to_keep_vec.size(), 0);

  // Convert colors_to_keep_vec to const reference for search
  const std::vector<PixelType> &tree = colors_to_keep_vec;

  // For each color to remove, find the nearest neighbor
  for (const auto &color_remove : colors_to_remove) {
    double min_distance = std::numeric_limits<double>::max();
    PixelType nearest_color = color_remove;
    auto context = KDTreeSearchContext<PixelType>{tree, color_remove, min_distance, nearest_color};
    kdTreeNearestNeighbor<PixelType>(context, 0, tree.size(),0);

    replacement_map[color_remove] = nearest_color;
  }
}

// Function to remove least frequent colors
template<typename PixelType>
void removeColors(int num_colors_to_remove, std::vector<PixelType> &pixels) {
  std::unordered_map<PixelType, size_t> color_frequencies;
  for (const auto &pixel : pixels) {// Count frequencies
    ++color_frequencies[pixel];}
  std::vector<std::pair<PixelType, size_t>> color_freq_vec(color_frequencies.begin(), color_frequencies.end());// Create a vector of colors sorted by frequency
  std::sort(color_freq_vec.begin(), color_freq_vec.end(), [](const std::pair<PixelType, size_t> &pix_a, const std::pair<PixelType, size_t> &pix_b) {

    if (pix_a.second != pix_b.second) { return pix_a.second < pix_b.second;} // Sort by frequency
    if (pix_a.first.blue != pix_b.first.blue) { return pix_a.first.blue > pix_b.first.blue;}// Tie-breaker by color components
        if (pix_a.first.green != pix_b.first.green) { return pix_a.first.green > pix_b.first.green;}
        return pix_a.first.red > pix_b.first.red;});

  size_t total_unique_colors = color_freq_vec.size();
  size_t const num_colors_to_actually_remove = std::min(static_cast<size_t>(num_colors_to_remove), total_unique_colors);
  std::unordered_set<PixelType> colors_to_remove;// Select colors to remove and keep
  std::vector<PixelType> colors_to_keep_vec;

  for (size_t i = 0; i < num_colors_to_actually_remove; ++i) {
    colors_to_remove.insert(color_freq_vec[i].first);}

  for (size_t i = num_colors_to_actually_remove; i < total_unique_colors; ++i) {
    colors_to_keep_vec.push_back(color_freq_vec[i].first);}

  std::unordered_map<PixelType, PixelType> replacement_map;// Create replacement map
  FindNearestColors<PixelType>(colors_to_remove, replacement_map, colors_to_keep_vec);

  for (auto &pixel : pixels) {// Replace colors in the image
    auto iterator = replacement_map.find(pixel);
    if (iterator != replacement_map.end()) {
      pixel = iterator->second;
    }
  }
}

// Main function to remove the least frequent colors from the image
void removeLeastFrequentColors(PPMImageAOS &image, int num_colors_to_remove);

#endif // CUTFREQAOS_HPP
