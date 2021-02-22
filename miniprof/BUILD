cc_library(
    name = "miniprof",
    srcs = glob(["*.cc"]),
    hdrs = glob(["*.hh"]),
    deps = [],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "miniprof_test",
    srcs = glob(["test/*.cc"]),
    size = "small",
    deps = [
        ":miniprof",
        "@gtest//:gtest",
        "@gtest//:gtest_main",
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "miniprof_benchmark",
    srcs = ["benchmark/miniprof_benchmark.cc"],
    deps = [
        ":miniprof",
        "@benchmark//:benchmark",
    ],
    visibility = ["//visibility:public"],
)
