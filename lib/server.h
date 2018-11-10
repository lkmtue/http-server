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

namespace lib {
namespace server {

const int MAX_CONCURRENT_CONNECTION = 16;
const int READ_BUF_SIZE = 16;
const int EPOLL_WAIT_MAX_EVENTS = 16;

class Server {
 public:

  Server(int port): port(port) {}

  void start();

 private:
  int port;
};
} // server
} // lib

#endif
