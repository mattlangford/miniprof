#include <chrono>
#include <iostream>
#include <thread>

#include "example/processing.hh"
#include "miniprof/miniprof.hh"

void run_convolution([[maybe_unused]] const char* name, size_t dim, Image& image) {
    for (size_t i = 0; i < 100; ++i) {
        profile(name);
        image += average_convolution(dim, image);
    }
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    init_default_profiler();
    profile_function();

    Image image = random(100, 200);

    // in the main thread
    run_convolution("conv3", 3, image);
    run_convolution("conv11", 11, image);
    run_convolution("conv25", 25, image);

    // in some secondary threads
    std::vector<std::thread> threads;
    std::vector<Image> outputs;
    constexpr size_t kNumThreads = 10;
    outputs.resize(kNumThreads, image);
    for (size_t i = 0; i < kNumThreads; ++i)
        threads.emplace_back([i, &outputs](){ run_convolution("thread_conv11", 11, outputs[i]); });
    for (auto& thread : threads)
        thread.join();

    {
        profile("sum_outputs");
        for (auto& output : outputs)
        {
            image += output;
        }
    }

    double bin_size = 0.01;

    std::vector<size_t> hist = histogram(bin_size, image);

    {
        profile("get_results");
        auto it = std::max_element(hist.begin(), hist.end());
        size_t bin = std::distance(hist.begin(), it);
        std::cout << "Max bin: " << bin << " (" << (bin * bin_size) << " -> " << (bin * bin_size + bin_size) << ")\n";
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Total execution took: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms\n";
}
