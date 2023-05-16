#include <fcntl.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>

#include <sys/epoll.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>

#define safe(expr)                                                              \
  ({                                                                            \
    typeof(expr) __tmp = expr;                                                  \
    if (__tmp == -1) {                                                          \
      printf("%s:%d "#expr" failed: %s\n", __FILE__, __LINE__, strerror(errno));\
      exit(EXIT_FAILURE);                                                       \
    }                                                                           \
    __tmp;                                                                      \
  })
#define loop for(;;)
#define find(init, cond) ({ int index = -1;  for (init) if (cond) { index = i; break; } index; })
#define repeat(n) for(int i = 0; i < n; i++)
#define print(x) write(STDOUT_FILENO, x, sizeof(x))