cc_library(
  name = "server",
  srcs = ["server.cc"],
  hdrs = ["server.h"],
  deps = [
    "//lib:event-queue",
    "//lib:event-types",
  ],
  visibility = ["//main:__pkg__"],
)

cc_library(
  name = "event-queue",
  srcs = ["event-queue.cc"],
  hdrs = ["event-queue.h"],
)

cc_library(
  name = "event-types",
  hdrs = ["event-types.h"],
)
