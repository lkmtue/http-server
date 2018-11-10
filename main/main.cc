#include <cstdio>
#include <memory>
#include <thread>
#include <iostream>

#include "lib/server.h"
#include "lib/event-queue.h"
#include "lib/event-types.h"

void startServer(lib::server::EventQueue *eventQueue) {
  lib::server::Server server(8080, eventQueue);
  server.start();
}

void eventLoop(lib::server::EventQueue *eventQueue) {
  while (1) {
    if (!eventQueue->empty()) {
      std::shared_ptr<lib::server::Event> event = eventQueue->pop();

      switch (event->eventType) {
        case NEW_CONNECTION_EVENT: {
          auto fd = (int *)event->eventInfo.get();
          std::cout << "[INFO] New connection: " << *fd << std::endl;
          break;
        }

        case READ_EVENT: {
          auto readEvent = (lib::server::ReadEvent *)event->eventInfo.get();
          std::cout << "[INFO] Read: " << std::endl << *readEvent->data << std::endl;
          break;
        }

        case CLOSE_EVENT:
          printf("Closed\n");
          break;

        default:
          break;
      }
    }
  }
}

int main() {
  lib::server::EventQueue *eventQueue = new lib::server::EventQueue();
  std::thread server(startServer, eventQueue);
  eventLoop(eventQueue);
}
