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
    profile("main");

    {
        // At any point in the program (including other files/functions), using the profile() macro can be used to
        // measure the runtime of the scope it's called in.
        profile("expensive_function");
        expensive_function():
    }
}
```
Building the above code with `-DENABLE_PROFILING` and running will produce a CSV file at `/tmp/prof`. The `analysis.py`
script can be used to view the results. Note that without defining `ENABLE_PROFILING`, there will be zero overhead and
no output file generated.
