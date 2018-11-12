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
#include <iostream>

namespace lib {
namespace server {
namespace {

void setSockaddr(sockaddr_in *addr, int port) {
  bzero((char *)addr, sizeof(sockaddr_in));
  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = INADDR_ANY;
  addr->sin_port = htons(port);
}

void epollCtlAdd(int epfd, int fd, unsigned int events) {
  epoll_event ev;
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

void reuseSocket(int serverSocket) {
  int enable = 1;
  if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
    perror("setsockopt(SO_REUSEADDR) failed");
    exit(1);

  }
}
}

void Server::start() {
  struct sockaddr_in srvAddr;

  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  setSockaddr(&srvAddr, port);

  reuseSocket(serverSocket);

  bind(serverSocket, (struct sockaddr *)&srvAddr, sizeof(srvAddr));
  setNonBlocking(serverSocket);

  listen(serverSocket, MAX_CONCURRENT_CONNECTION);

  epfd = epoll_create(1);
  epollCtlAdd(epfd, serverSocket, EPOLLIN | EPOLLOUT | EPOLLET);

  epollWait();
}

void Server::epollWait() {
  while (1) {
    int nfds = epoll_wait(epfd, events, EPOLL_WAIT_MAX_EVENTS, EPOLL_WAIT_TIME_OUT);
    for (int i = 0; i < nfds; i++) {
      int eventFd = events[i].data.fd;
      if (eventFd == serverSocket) {
        handleNewConnection();
      } else if (events[i].events & EPOLLIN) {
        handleReadEvent(eventFd);
      } else {
        printf("Unexpected\n");
      }
      if (events[i].events & (EPOLLRDHUP | EPOLLHUP)) {
        epoll_ctl(epfd, EPOLL_CTL_DEL, eventFd, NULL);
        close(eventFd);
        connectionHandler->onClose(eventFd);
        // taskQueue->push([this, eventFd]() { this->connectionHandler->onClose(eventFd); });
      }
    }
    // taskQueue->push([this]() { this->epollWait(); });
  }
}

void Server::handleNewConnection() {
  static struct sockaddr_in cliAddr;
  static socklen_t socklen = sizeof(cliAddr);

  int connSock = accept(serverSocket, (struct sockaddr *)&cliAddr, &socklen);
  setNonBlocking(connSock);
  epollCtlAdd(epfd, connSock, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP);

  connectionHandler->onConnect(connSock);

  // taskQueue->push([this, connSock]() { this->connectionHandler->onConnect(connSock); });
}

void Server::handleReadEvent(int eventFd) {
  static char buf[READ_BUF_SIZE];

  std::string s;

  while (1) {
    bzero(buf, sizeof(buf));
    int n = read(eventFd, buf, sizeof(buf));
    if (n <= 0) {
      break;
    } else {
      for (int i = 0; i < n; i++) {
        s += buf[i];
      }
    }
  }
  connectionHandler->onRead(eventFd, s);
  // taskQueue->push([this, eventFd, s] { this->connectionHandler->onRead(eventFd, s); });
}
} // server
} // lib
