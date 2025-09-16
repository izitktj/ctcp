#include <stdio.h>

#ifdef __linux__
  #include <sys/socket.h>
  #include <unistd.h>
#endif

#define PORT 8080

int main(int argc, char **argv) {
  printf("Hello, world!");

  return 0;
}
