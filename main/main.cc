#include "lib/server.h"
#include "lib/task-queue.h"
#include "lib/http.h"

void run(lib::task::TaskQueue *queue) {
  while (1) {
    auto task = queue->pop();
    task();
  }
}

lib::http::Response handleRequest(const lib::http::Request &request) {
  lib::http::Response response;
  response.code = 200;
  response.message = "OK";
  response.additionalHeaders.push_back({ "Content-Type", "text/html; charset=UTF-8" });
  response.body += "<html>";
  response.body += "<body>";
  response.body += "<div>" + request.method + "</div>";
  response.body += "<div>" + request.path + "</div>";
  response.body += "<div>" + request.body + "</div>";
  response.body += "</body>";
  response.body += "</html>";
  return response;
}

int main() {
  auto queue = new lib::task::TaskQueue();

  lib::server::Server server(8080, queue, handleRequest);
  server.start();

  run(queue);
}
