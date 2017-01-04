cc_library(
    name = "optonly",
    hdrs = ['src/midout.h'],
    includes = ['src'],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "midout",
    srcs = ['src/midout.cpp'],
    deps = [':optonly'],
    visibility = ["//visibility:public"],
)

# vim: ft=python
