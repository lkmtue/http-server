#include "lib/event-queue.h"

namespace lib {
namespace server {

using std::shared_ptr;

void EventQueue::push(shared_ptr<Event> e) {
  mtx.lock();
  queue.push(e);
  mtx.unlock();
}

shared_ptr<Event> EventQueue::pop() {
  mtx.lock();
  shared_ptr<Event> res = queue.front();
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
