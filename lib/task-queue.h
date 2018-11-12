#ifndef LIB_TASK_QUEUE
#define LIB_TASK_QUEUE

#include <functional>
#include <memory>
#include <mutex>
#include <queue>

namespace lib {
namespace task {

typedef std::function<void()> Task;

class TaskQueue {
 public:

  TaskQueue() {}

  void push(Task task) {
    mtx.lock();
    q.push(task);
    mtx.unlock();
  }

  Task pop() {
    while (1) {
      mtx.lock();
      if (!q.empty()) {
        auto res = std::move(q.front());
        q.pop();
        mtx.unlock();
        return res;
      }
      mtx.unlock();
    }
  }

 private:
  std::mutex mtx;
  std::queue<Task> q;
};
} // server
} // lib

#endif
