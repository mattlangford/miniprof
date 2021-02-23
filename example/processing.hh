#include <vector>

struct Image {
    size_t width;
    size_t height;
    std::vector<float> data;

    float at(size_t row, size_t col) const;
    float& at(size_t row, size_t col);

    Image& operator+=(const Image& rhs);
};

Image random(size_t width, size_t height);

Image average_convolution(size_t size, const Image& image);

void normalize(Image& image);

std::vector<size_t> histogram(float bin_size, const Image& image, float& min_value);
