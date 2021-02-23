cc_binary(
    name = "example",
    srcs = ["example.cc"],
    deps = [
        "//miniprof",
        ":processing"
    ],
)

cc_library(
    name = "processing",
    srcs = ["processing.cc"],
    hdrs = ["processing.hh"],
    deps = [
        "//miniprof",
    ],
)
