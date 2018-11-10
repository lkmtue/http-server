#ifndef LIB_CONNECTION_HANDLER
#define LIB_CONNECTION_HANDLER

#include <memory>
#include <mutex>
#include <queue>

namespace lib {
namespace server {

class Event {
 public:
  Event(int eventType, const std::shared_ptr<void> &eventInfo): eventType(eventType), eventInfo(eventInfo) {}

  int eventType;
  const std::shared_ptr<void> eventInfo;
};

class EventQueue {
 public:

  EventQueue() {}

  void push(int eventType, const std::shared_ptr<void> &eventInfo);

  std::shared_ptr<Event> pop();

  bool empty();

 private:
  std::mutex mtx;
  std::queue<std::shared_ptr<Event>> queue;
};
} // server
} // lib

#endif
