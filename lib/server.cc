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

void reuseSocket(int listenSock) {
  int enable = 1;
  if (setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
    perror("setsockopt(SO_REUSEADDR) failed");
    exit(1);
  }
}

inline void handleNewConnection(int listenSock, int epfd) {
  static struct sockaddr_in cliAddr;
  static socklen_t socklen = sizeof(cliAddr);

  int connSock = accept(listenSock, (struct sockaddr *)&cliAddr, &socklen);

  setNonBlocking(connSock);

  epollCtlAdd(epfd, connSock, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP);
}

inline void handleReadEvent(int eventFd) {
  static char buf[READ_BUF_SIZE];
  while (1) {
    bzero(buf, sizeof(buf));
    int n = read(eventFd, buf, sizeof(buf));
    if (n <= 0) {
      break;
    } else {
      printf("[+] data: %s\n", buf);
      write(eventFd, buf, strlen(buf));
    }
  }
}
}

void Server::start() {
  struct sockaddr_in srvAddr;

  int listenSock = socket(AF_INET, SOCK_STREAM, 0);
  setSockaddr(&srvAddr, port);

  reuseSocket(listenSock);

  bind(listenSock, (struct sockaddr *)&srvAddr, sizeof(srvAddr));
  setNonBlocking(listenSock);

  listen(listenSock, maxConn);

  int epfd = epoll_create(1);

  epollCtlAdd(epfd, listenSock, EPOLLIN | EPOLLOUT | EPOLLET);

  epoll_event events[EPOLL_WAIT_MAX_EVENTS];

  while (1) {
    int nfds = epoll_wait(epfd, events, EPOLL_WAIT_MAX_EVENTS, -1);
    for (int i = 0; i < nfds; i++) {
      if (events[i].data.fd == listenSock) {
        handleNewConnection(listenSock, epfd);
      } else if (events[i].events & EPOLLIN) {
        handleReadEvent(events[i].data.fd);
      } else {
        printf("Unexpected\n");
      }
      if (events[i].events & (EPOLLRDHUP | EPOLLHUP)) {
        printf("[+] connection closed\n");
        epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
        close(events[i].data.fd);
      }
    }
  }
}
} // server
} // lib
