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
There is a simple benchmark in the `miniprof` folder, which shows relatively small differences with and without profiling.  Additionally with the program in the `example` folder (which is a bit more complicated and realistic with multiple threads), I've seen differences in the noise between the with and without profiling runs.

## Analysis
Here is an example analysis:
```
# Generate the profiling data
$ bazel run //example --copt=-DENABLE_PROFILING

# Analyze the generated data
$ python3 analyze.py

====================================================================================================================
Profiling Results (note that results are similar to microseconds but may not be exact, relative times are accurate):
====================================================================================================================
                     microseconds                                    
                              sum count       mean     std     p99.00
name                                                                 
main                      4244238     1 4244238.00     nan 4244238.00
thread_conv11             3807751   500    7615.50  647.36    8871.18
conv25                    2951683   100   29516.83 1546.36   33817.62
conv11                     491144   100    4911.44  261.68    5579.33
normalize                   51945  1605      32.36   11.57      60.00
operator+=()                34660   805      43.06   14.06      81.00
conv3                       31566   100     315.66   45.24     417.61
normalize()/apply           26005  1605      16.20    4.08      30.00
histogram()/find_max        25512  1606      15.89    9.11      38.75
random                        512     1     512.00     nan     512.00
sum_outputs                   269     1     269.00     nan     269.00
histogram                      68     1      68.00     nan      68.00
histogram()/binning            53     1      53.00     nan      53.00
get_results                    50     1      50.00     nan      50.00
```
Here it shows some statistics about the collected data (note the `nan`s for standard deviation where only one sample is present). As the note at the top of the report says, the times may not be exactly microseconds (since the clock source may count CPU ticks rather than timestamps), but the relative magnitudes are still accurate and useful.


