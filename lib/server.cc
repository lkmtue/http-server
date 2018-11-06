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

#ifndef LIB_SERVER
#define LIB_SERVER

namespace lib {
namespace server {

void Server::start() {
  listenSock = socket(AF_INET, SOCK_STREAM, 0);
  set_sockaddr(&srvAddr);

  bind(listenSock, (struct sockaddr *)&srvAddr, sizeof(srvAddr));
  setNonBlocking(listenSock);

  listen(listenSock, MAX_CONN);
}

int Server::setNonBlocking(int sockfd) {
  if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) == -1) {
    return -1;
  }
  return 0;
}

} // server
} // lib
