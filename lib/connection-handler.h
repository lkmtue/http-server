#ifndef LIB_CONNECTION_HANDLER
#define LIB_CONNECTION_HANDLER

#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include <algorithm>
#include <string>

#include "lib/http.h"

namespace lib {
namespace server {

const char END_OF_HEADERS[] = "\r\n\r\n";

namespace parse {
const int FAILED = -1;
const int PARSING = 0;
const int SUCCESS = 1;
} // parse

class ConnectionData {
 public:
  int fd;
  std::string body;

  int parseStatus = 0;
  std::string method;
  std::string path;
  std::vector<std::pair<std::string, std::string>> headers;

  ConnectionData() {}
  ConnectionData(int fd): fd(fd) {}

  void onRead(char c) {
    if (matched < 4) {
      if (END_OF_HEADERS[matched] == c) {
        matched++;
      } else {
        matched = 0;
      }
      if (c == '\n' && matched < 4) {
        lines.push_back(line);
        line = "";
      } else {
        if (c != '\r' && c != '\n') {
          line += c;
        }
      }
      if (matched == 4) {
        if (!parseHeaders()) {
          parseStatus = parse::FAILED;
        } else {
          if (contentLength == 0) {
            parseStatus = parse::SUCCESS;
          }
        }
      }
    } else {
      if ((int)body.size() < contentLength) {
        body += c;
        if ((int)body.size() == contentLength) {
          parseStatus = parse::SUCCESS;
        }
      }
    }
  }

 private:
  int matched = 0;
  std::string line;
  std::vector<std::string> lines;
  int contentLength = 0;

  bool parseHeaders() {
    if (lines.size() < 2) {
      return false;
    }
    std::vector<std::string> words;
    splitSpaces(lines[0], &words);

    if (words.size() != 3) {
      return false;
    }

    method = words[0];
    path = words[1];

    for (int i = 1; i < (int)lines.size(); i++) {
      bool found = false;
      for (int j = 0; j + 1 < (int)lines[i].size(); j++) {
        if (lines[i][j] == ':' && lines[i][j + 1] == ' ') {
          found = true;
          headers.push_back(make_pair(lines[i].substr(0, j), lines[i].substr(j + 2)));
          break;
        }
      }
      if (!found) {
        return false;
      }
    }

    for (auto &header: headers) {
      if (header.first == "Content-Length") {
        if (!stringToInt(header.second, &contentLength)) {
          return false;
        }
      }
    }

    return true;
  }

  void splitSpaces(const std::string &s, std::vector<std::string> *words) {
    std::string word = "";
    for (int i = 0; i <= (int)s.size(); i++) {
      if (i == (int)s.size() || s[i] == ' ') {
        words->push_back(word);
        word = "";
      } else  {
        word += s[i];
      }
    }
  }

  bool stringToInt(const std::string &s, int *x) {
    *x = 0;
    for (char c: s) {
      if (c < '0' || c > '9') return false;
      *x = *x * 10 + c - '0';
    }
    return true;
  }
};

class ConnectionHandler {
 public:
  ConnectionHandler(lib::http::HttpRequestHandler httpRequestHandler): httpRequestHandler(httpRequestHandler) {}

  void onConnect(int fd) {
    connections[fd] = ConnectionData(fd);
  }

  void onRead(int fd, const std::string &data) {
    auto &conn = connections[fd];
    for (char c: data) {
      if (conn.parseStatus != parse::PARSING)  {
        responseFailed(fd);
        return;
      }
      conn.onRead(c);
    }
    if (conn.parseStatus != parse::PARSING) {
      if (conn.parseStatus == parse::SUCCESS) {
        handleRequest(fd);
      } else {
        responseFailed(fd);
      }
    }
  }

  void onClose(int fd) {
    auto it = connections.find(fd);
    if (it != connections.end()) {
      connections.erase(it);
    }
  }

 private:
  std::unordered_map<int, ConnectionData> connections;
  lib::http::HttpRequestHandler httpRequestHandler;

  void handleRequest(int fd) {
    auto &conn = connections[fd];

    lib::http::Request request;
    request.method = conn.method;
    request.path = conn.path;
    request.headers = conn.headers;
    request.body = conn.body;

    lib::http::Response response = httpRequestHandler(request);

    responseHttp(fd, response);
  }

  // TODO: Specify failure code.
  void responseFailed(int fd) {
    lib::http::Response response;
    response.code = 500;
    response.message = "Internal server error";
    responseHttp(fd, response);
  }

  void responseHttp(int fd, const lib::http::Response &response) {
    responseString(fd, "HTTP/1.1 " + intToString(response.code) + " " + response.message + "\r\n");
    responseString(fd, "Content-Length: " + intToString(response.body.size()) + "\r\n");
    for (const auto &header: response.additionalHeaders) {
      responseString(fd, header.first + ": " + header.second + "\r\n");
    }
    responseString(fd, "\r\n");
    responseString(fd, response.body);
    onClose(fd);
  }

  void responseString(int fd, const std::string &s) {
    const char *buf = s.c_str();
    int remain = s.size();
    while (remain) {
      int n = write(fd, buf, remain);
      if (n != -1) {
        remain -= n;
        buf += n;
      } else {
        remain = 0;
      }
    }
  }

  std::string intToString(int x) {
    std::string res = "";
    while (x) {
      res += char(x % 10 + '0');
      x /= 10;
    }
    reverse(res.begin(), res.end());
    return res;
  }
};
} // server
} // lib
#endif
