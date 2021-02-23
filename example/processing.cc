#include "example/processing.hh"

#include <algorithm>
#include <iostream>
#include <random>

#include "miniprof/miniprof.hh"

//
// #############################################################################
//

float Image::at(size_t row, size_t col) const { return data.at(row * width + col); }

//
// #############################################################################
//

float& Image::at(size_t row, size_t col) { return data.at(row * width + col); }

//
// #############################################################################
//

Image& Image::operator+=(const Image& rhs) {
    if (rhs.data.size() != data.size()) return *this;

    profile("operator+=()");

    for (size_t i = 0; i < data.size(); ++i) data[i] += rhs.data[i];

    return *this;
}

//
// #############################################################################
//

Image random(size_t width, size_t height) {
    Image image;
    image.height = height;
    image.width = width;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    const size_t total_size = height * width;
    image.data.resize(total_size, 0.f);
    for (float& data : image.data) {
        data = dist(gen);
    }
    return image;
}

//
// #############################################################################
//

Image average_convolution(size_t size, const Image& image) {
    Image result;
    result.height = image.height;
    result.width = image.width;
    result.data.resize(image.data.size(), 0.f);

    // Switching to int here since we'll be going into the negatives during the following routine
    const int width = static_cast<int>(image.width);
    const int height = static_cast<int>(image.height);
    const int half_size = static_cast<int>(size) / 2;
    float inv_conv_area = 1.f / (size * size);

    for (int col = 0; col < height; ++col) {
        for (int row = 0; row < width; ++row) {
            float& result_px = result.at(row, col);
            for (int conv_col = col - half_size; conv_col < col + half_size; ++conv_col) {
                for (int conv_row = row - half_size; conv_row < row + half_size; ++conv_row) {
                    if (conv_row < 0 || conv_col < 0 || conv_row >= width || conv_col >= height) {
                        // Effectively adding zero
                        continue;
                    }

                    result_px += inv_conv_area * image.at(conv_row, conv_col);
                }
            }
        }
    }
    return result;
}

//
// #############################################################################
//

void normalize(Image& image) {
    float max_value = 0.f;
    float min_value = 0.f;

    {
        profile("normalize()/find_minmax");
        auto [min_it, max_it] = std::minmax_element(image.data.begin(), image.data.end());
        min_value = *min_it;
        max_value = *max_it;
    }

    float range = max_value - min_value;

    {
        profile("normalize()/apply");
        for (float& data : image.data) {
            data = (data - min_value) / range;
        }
    }
}

//
// #############################################################################
//

std::vector<size_t> histogram(float bin_size, const Image& image, float& min_value) {
    float max_value = 0.f;

    {
        profile("histogram()/find_minmax");
        auto [min_it, max_it] = std::minmax_element(image.data.begin(), image.data.end());
        min_value = *min_it;
        max_value = *max_it;
    }

    std::vector<size_t> bins;
    bins.resize((max_value - min_value) / bin_size, 0);

    {
        profile("histogram()/binning");
        for (const float data : image.data) {
            size_t bin = (data - min_value) / bin_size;
            bins[bin]++;
        }
    }

    return bins;
}
