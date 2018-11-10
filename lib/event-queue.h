#ifndef LIB_CONNECTION_HANDLER
#define LIB_CONNECTION_HANDLER

#include <memory>
#include <mutex>
#include <queue>

namespace lib {
namespace server {

class Event {
 public:
  Event(int eventType, std::shared_ptr<void *>eventInfo):
   eventType(eventType), eventInfo(eventInfo) {}

 private:
  int eventType;
  std::shared_ptr<void *> eventInfo;
};

class EventQueue {
 public:

  EventQueue() {}

  void push(std::shared_ptr<Event> e);

  std::shared_ptr<Event> pop();

  bool empty();

 private:
  std::mutex mtx;
  std::queue<std::shared_ptr<Event>> queue;
};
} // server
} // lib

#endif

