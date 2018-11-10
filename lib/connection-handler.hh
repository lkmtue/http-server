#ifndef LIB_CONNECTION_HANDLER
#define LIB_CONNECTION_HANDLER

namespace lib {
namespace server {

class ConnectionHandler {
 public:
  ConnectionHandler() {}

  void onConnect(int fd): fd(fd);

  void onRead(int len, char *buf);

  void onClose();

 private:
  int fd;

};
} // server
} // lib

#endif
