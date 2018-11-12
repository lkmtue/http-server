#ifndef LIB_HTTP
#define LIB_HTTP

#include <vector>
#include <string>
#include <functional>

namespace lib {
namespace http {

class Request {
 public:
  std::string path;
  std::string method;
  std::vector<std::pair<std::string, std::string>> headers;
  std::string body;

  Request() {}
};

class Response {
 public:
  int code;
  std::string message;
  std::vector<std::pair<std::string, std::string>> additionalHeaders;
  std::string body;

  Response() {}
};

typedef std::function<Response(const Request &)> HttpRequestHandler;
} // server
} // lib
#endif
