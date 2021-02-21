cc_library(
    name = "miniprof",
    srcs = glob(["miniprof/*.cc"]),
    hdrs = glob(["miniprof/*.hh"]),
    deps = [],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "miniprof_test",
    srcs = glob(["test/*.cc"]),
    deps = [
        ":miniprof",
        "@gtest//:gtest",
        "@gtest//:gtest_main",
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "global_benchmark",
    srcs = ["benchmark/global_benchmark.cc"],
    deps = [
        ":miniprof",
        "@benchmark//:benchmark",
    ],
    visibility = ["//visibility:public"],
)
