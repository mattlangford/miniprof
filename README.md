# Miniprof

Small little library used for profiling multithreaded code with a relatively small amount of overhead in addition to tools for analyzing the results.

## Usage
For concrete usage check out the `example` directory.

Generally, usage looks like:
```
#include "miniprof/miniprof.hh"

int main()
{
    // Important to initialize the profiler at the start of execution.
    init_default_profiler();

    // Profile the whole execution of the program.
    profile_function();

    {
        // At any point in the program (including other files/functions), using the profile() macro can be used to
        // measure the runtime of the scope it's called in.
        profile("expensive_function");
        expensive_function():
    }
}
```
Building the above code with `-DENABLE_PROFILING` (and ideally -O3) and running will produce a CSV file at `/tmp/prof`. The `analysis.py` script can be used to view the results. Note that without defining `ENABLE_PROFILING`, there will be zero overhead and no output file generated.

## Performance
There is a simple benchmark in the `miniprof` folder, which shows relatively small differences with and without profiling:
```
--------------------------------------------------------------
Benchmark                    Time             CPU   Iterations
--------------------------------------------------------------
no_profiling/2            7659 ns         3665 ns       181812
no_profiling/10          18000 ns         3847 ns       178484
no_profiling/100        133823 ns         6277 ns       111889
no_profiling/1000      1227101 ns        28773 ns        10000
with_profiling/2          7890 ns         3915 ns       182527
with_profiling/10        18152 ns         4031 ns       172560
with_profiling/100      133909 ns         6440 ns       110269
with_profiling/1000    1227856 ns        33746 ns        10000
```
More complicated benchmarks may come in the future.

Additionally with the example in the `example` folder (which is a bit more complicated/realistic), there are negligible differences:
```
$ time bazel run //example --copt=-DENABLE_PROFILING
...
bazel run //example --copt=-DENABLE_PROFILING  3.99s user 0.07s system 97% cpu 4.170 total
```
```
$ time bazel run //example
...
bazel run //example 3.97s user 0.06s system 96% cpu 4.194 total
```
Of course your results may vary.
