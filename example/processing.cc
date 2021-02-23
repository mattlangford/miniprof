#include "example/processing.hh"

#include <algorithm>
#include <iostream>
#include <random>

#include "miniprof/miniprof.hh"

//
// #############################################################################
//

double Image::at(size_t row, size_t col) const { return data.at(row * width + col); }

//
// #############################################################################
//

double& Image::at(size_t row, size_t col) { return data.at(row * width + col); }

//
// #############################################################################
//

Image& Image::operator+=(const Image& rhs) {
    if (rhs.data.size() != data.size()) return *this;

    profile("operator+=()");

    for (size_t i = 0; i < data.size(); ++i) data[i] += rhs.data[i];

    normalize(*this);
    return *this;
}

//
// #############################################################################
//

Image random(size_t width, size_t height) {
    profile_function();
    Image image;
    image.height = height;
    image.width = width;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    const size_t total_size = height * width;
    image.data.resize(total_size, 0.0);
    for (double& data : image.data) {
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
    result.data.resize(image.data.size(), 0.0);

    // Switching to int here since we'll be going into the negatives during the following routine
    const int width = static_cast<int>(image.width);
    const int height = static_cast<int>(image.height);
    const int half_size = static_cast<int>(size) / 2;

    for (int col = 0; col < height; ++col) {
        for (int row = 0; row < width; ++row) {
            double& result_px = result.at(row, col);

            for (int conv_col = col - half_size; conv_col < col + half_size; ++conv_col) {
                for (int conv_row = row - half_size; conv_row < row + half_size; ++conv_row) {
                    if (conv_row < 0 || conv_col < 0 || conv_row >= width || conv_col >= height) {
                        // Effectively adding zero
                        continue;
                    }

                    result_px += image.at(conv_row, conv_col);
                }
            }
            result_px /= size * size;
        }
    }

    normalize(result);
    return result;
}

//
// #############################################################################
//

void normalize(Image& image) {
    profile_function();

    double max_value = 0.0;

    {
        profile("histogram()/find_max");
        max_value = *std::max_element(image.data.begin(), image.data.end());
    }

    {
        profile("normalize()/apply");
        for (double& data : image.data) {
            data /= max_value;
        }
    }
}

//
// #############################################################################
//

std::vector<size_t> histogram(double bin_size, const Image& image) {
    profile_function();
    double max_value = 0;

    {
        profile("histogram()/find_max");
        max_value = *std::max_element(image.data.begin(), image.data.end());
    }

    std::vector<size_t> bins((max_value / bin_size) + 1, 0);

    {
        profile("histogram()/binning");
        for (const double data : image.data) {
            size_t bin = data / bin_size;
            bins.at(bin)++;
        }
    }

    return bins;
}
