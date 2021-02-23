#include <chrono>
#include <iostream>

#include "example/processing.hh"
#include "miniprof/miniprof.hh"

void run_convolution([[maybe_unused]] const char* name, size_t dim, Image& image) {
    for (size_t i = 0; i < 100; ++i) {
        {
            profile(name);
            image += average_convolution(dim, image);
        }

        {
            profile("normalize");
            normalize(image);
        }
    }
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    init_default_profiler();
    profile_function();

    Image image;
    {
        profile("random");
        image = random(100, 200);
    }

    run_convolution("conv3", 3, image);
    run_convolution("conv11", 11, image);
    run_convolution("conv25", 25, image);

    std::vector<size_t> hist;
    float bin_size = 0.1f;
    float min_value = 0.f;
    {
        profile("hist");
        hist = histogram(bin_size, image, min_value);
    }

    {
        profile("print_results");
        for (size_t bin = 0; bin < hist.size(); ++bin) {
            if (hist[bin] == 0) continue;

            std::cout << "bin " << bin << " (" << (min_value + bin * bin_size) << " -> "
                      << (min_value + (bin + 1) * bin_size) << "): " << hist[bin] << "\n";
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Total execution took: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms\n";
}
