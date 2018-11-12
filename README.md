# http-server
A basic http-server written in C++ from scratch.
# How to run
* Install https://bazel.build/
* `git clone https://github.com/lkmtue/http-server.git`
* `bazel run //main:main`
* `curl http://localhost:8080`
# Scope
* `Content-length` must be presented in headers so that the length of the body can be pre-determined.
