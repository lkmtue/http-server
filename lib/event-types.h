#ifndef LIB_EVENTS_TYPE
#define LIB_EVENTS_TYPE

#include <memory>
#include <mutex>
#include <queue>
#include <string>

namespace lib {
namespace server {

#define NEW_CONNECTION_EVENT 0

#define READ_EVENT 1

class ReadEvent {
 public:
  ReadEvent(int fd, const std::shared_ptr<std::string> &data): fd(fd), data(data) {}

  int fd;
  const std::shared_ptr<std::string> data;
};

#define CLOSE_EVENT 2

} // server
} // lib
#endif
