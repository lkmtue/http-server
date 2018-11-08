#include <cstdio>

#include "lib/server.h"

int main() {
  lib::server::Server server(8080, 16);
  server.start();
}
