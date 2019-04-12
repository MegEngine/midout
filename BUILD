package(default_visibility = ["//visibility:public"])

# Empty cc_library for backward compatibility
cc_library(
    name = "optonly",
    hdrs = ['src/midout.h'],
    includes = ['src'],
    deprecation = "//brain/midout:optonly is (and was) effectively a no-op, please remove such deps",
)

cc_library(
    name = "midout",
    srcs = ['src/midout.cpp'],
    hdrs = ['src/midout.h'],
    includes = ['src'],
)

# vim: ft=python
