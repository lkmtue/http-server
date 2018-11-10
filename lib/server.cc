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

#include "lib/event-queue.h"

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

inline void handleNewConnection(int serverSocket, int epfd) {
  static struct sockaddr_in cliAddr;
  static socklen_t socklen = sizeof(cliAddr);

  int connSock = accept(serverSocket, (struct sockaddr *)&cliAddr, &socklen);

  setNonBlocking(connSock);

  epollCtlAdd(epfd, connSock, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP);
  printf("\n[+] new connection\n");

}

inline void handleReadEvent(int eventFd) {
  static char buf[READ_BUF_SIZE];
  while (1) {
    bzero(buf, sizeof(buf));
    int n = read(eventFd, buf, sizeof(buf));
    if (n <= 0) {
      break;
    } else {
      printf("%s", buf);
      fflush(stdout);
    }
  }
}
}

void Server::start() {
  struct sockaddr_in srvAddr;

  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  setSockaddr(&srvAddr, port);

  reuseSocket(serverSocket);

  bind(serverSocket, (struct sockaddr *)&srvAddr, sizeof(srvAddr));
  setNonBlocking(serverSocket);

  listen(serverSocket, MAX_CONCURRENT_CONNECTION);

  int epfd = epoll_create(1);

  epollCtlAdd(epfd, serverSocket, EPOLLIN | EPOLLOUT | EPOLLET);

  epoll_event events[EPOLL_WAIT_MAX_EVENTS];

  while (1) {
    int nfds = epoll_wait(epfd, events, EPOLL_WAIT_MAX_EVENTS, -1);
    for (int i = 0; i < nfds; i++) {
      if (events[i].data.fd == serverSocket) {
        handleNewConnection(serverSocket, epfd);
      } else if (events[i].events & EPOLLIN) {
        handleReadEvent(events[i].data.fd);
      } else {
        printf("Unexpected\n");
      }
      if (events[i].events & (EPOLLRDHUP | EPOLLHUP)) {
        printf("\n[+] connection closed\n");
        epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
        close(events[i].data.fd);
      }
    }
  }
}
} // server
} // lib
