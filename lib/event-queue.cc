#include "lib/event-queue.h"

namespace lib {
namespace server {

void EventQueue::push(int eventType, const std::shared_ptr<void> &eventInfo) {
  mtx.lock();
  queue.push(std::shared_ptr<Event>(new Event(eventType, eventInfo)));
  mtx.unlock();
}

std::shared_ptr<Event> EventQueue::pop() {
  mtx.lock();
  std::shared_ptr<Event> res = std::move(queue.front());
  queue.pop();
  mtx.unlock();
  return res;
}

bool EventQueue::empty() {
  mtx.lock();
  bool res = queue.empty();
  mtx.unlock();
  return res;
}
} // server
} // lib
