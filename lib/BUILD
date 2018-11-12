cc_library(
  name = "server",
  srcs = ["server.cc"],
  hdrs = ["server.h"],
  deps = [
    "//lib:connection-handler",
  ],
  visibility = ["//main:__pkg__"],
)

cc_library(
  name = "connection-handler",
  hdrs = ["connection-handler.h"],
  deps = [
    "//lib:http",
  ],
)

cc_library(
  name = "http",
  hdrs = ["http.h"],
  visibility = ["//main:__pkg__"],
)
