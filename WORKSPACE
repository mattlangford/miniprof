load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "gtest",
    remote = "https://github.com/google/googletest",
    # If I use branch = "v1.10.x" I get warnings that say I should use these two lines instead
    commit = "703bd9caab50b139428cea1aaff9974ebee5742e",
    shallow_since = "1570114335 -0400",
)

git_repository(
    name = "benchmark",
    remote = "https://github.com/google/benchmark/",
    # tag = "v1.5.2",
    commit = "73d4d5e8d6d449fc8663765a42aa8aeeee844489",
    shallow_since = "1599818118 +0100"
)
