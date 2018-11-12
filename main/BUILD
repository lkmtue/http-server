cc_binary(
  name = "main",
  srcs = ["main.cc"],
  deps = [
    "//lib:server",
    "//lib:task-queue",
    "//lib:http",
  ],
  linkopts = [
    "-pthread"
  ],
)
