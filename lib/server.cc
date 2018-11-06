#include "server.h"

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
namespace {

void setSockaddr(struct sockaddr_in *addr, int port) {
  bzero((char *)addr, sizeof(struct sockaddr_in));
  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = INADDR_ANY;
  addr->sin_port = htons(port);
}

void epollCtlAdd(int epfd, int fd, uint32_t events) {
  struct epoll_event ev;
  ev.events = events;
  ev.data.fd = fd;
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    perror("epoll_ctl()\n");
    exit(1);
  }
}

int setNonBlocking(int sockfd) {
  if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) == -1) {
    return -1;
  }
  return 0;
}
}

void Server::start() {
  listenSock = socket(AF_INET, SOCK_STREAM, 0);
  setSockaddr(&srvAddr, port);

  bind(listenSock, (struct sockaddr *)&srvAddr, sizeof(srvAddr));
  setNonBlocking(listenSock);

  listen(listenSock, maxConn);
  epfd = epoll_create(1);

  epollCtlAdd(epfd, listenSock, EPOLLIN | EPOLLOUT | EPOLLET);

  socklen = sizeof(cliAddr);
}
} // server
} // lib
