#ifndef LIB_SERVER
#define LIB_SERVER

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "lib/connection-handler.h"
#include "lib/http.h"

namespace lib {
namespace server {

const int MAX_CONCURRENT_CONNECTION = 1000;
const int READ_BUF_SIZE = 1000;
const int EPOLL_WAIT_MAX_EVENTS = 1000;
const int EPOLL_WAIT_TIME_OUT = -1;

class Server {
 public:

  Server(
    int port,
    lib::http::HttpRequestHandler httpRequestHandler
  ): port(port), httpRequestHandler(httpRequestHandler) {
    connectionHandler = new ConnectionHandler(httpRequestHandler);
  }

  ~Server() {
    delete connectionHandler;
  }

  void start();

 private:

  int port;
  int serverSocket;
  int epfd;


  lib::server::ConnectionHandler *connectionHandler;

  lib::http::HttpRequestHandler httpRequestHandler;

  epoll_event events[EPOLL_WAIT_MAX_EVENTS];

  void epollWait();
  void handleNewConnection();
  void handleReadEvent(int eventFd);
};
} // server
} // lib
#endif
