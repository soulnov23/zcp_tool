#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/un.h>

#include <dlfcn.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <errno.h>
#include <netinet/in.h>
#include <time.h>

#include <pthread.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <resolv.h>
#include <map>

#include <time.h>
typedef int (*socket_pfn_t)(int domain, int type, int protocol);
typedef int (*connect_pfn_t)(int socket, const struct sockaddr *address,
                             socklen_t address_len);
typedef int (*close_pfn_t)(int fd);

typedef ssize_t (*read_pfn_t)(int fildes, void *buf, size_t nbyte);
typedef ssize_t (*write_pfn_t)(int fildes, const void *buf, size_t nbyte);

typedef ssize_t (*sendto_pfn_t)(int socket, const void *message, size_t length,
                                int flags, const struct sockaddr *dest_addr,
                                socklen_t dest_len);

typedef ssize_t (*recvfrom_pfn_t)(int socket, void *buffer, size_t length,
                                  int flags, struct sockaddr *address,
                                  socklen_t *address_len);

typedef ssize_t (*send_pfn_t)(int socket, const void *buffer, size_t length,
                              int flags);
typedef ssize_t (*recv_pfn_t)(int socket, void *buffer, size_t length,
                              int flags);

typedef int (*poll_pfn_t)(struct pollfd fds[], nfds_t nfds, int timeout);
typedef int (*setsockopt_pfn_t)(int socket, int level, int option_name,
                                const void *option_value, socklen_t option_len);

typedef int (*fcntl_pfn_t)(int fildes, int cmd, ...);
typedef struct tm *(*localtime_r_pfn_t)(const time_t *timep, struct tm *result);

typedef void *(*pthread_getspecific_pfn_t)(pthread_key_t key);
typedef int (*pthread_setspecific_pfn_t)(pthread_key_t key, const void *value);

typedef int (*pthread_rwlock_rdlock_pfn_t)(pthread_rwlock_t *rwlock);
typedef int (*pthread_rwlock_wrlock_pfn_t)(pthread_rwlock_t *rwlock);
typedef int (*pthread_rwlock_unlock_pfn_t)(pthread_rwlock_t *rwlock);

typedef int (*setenv_pfn_t)(const char *name, const char *value, int overwrite);
typedef int (*unsetenv_pfn_t)(const char *name);
typedef char *(*getenv_pfn_t)(const char *name);

typedef hostent *(*gethostbyname_pfn_t)(const char *name);
typedef int (*gethostbyname_r_pfn_t)(const char *__restrict name,
                                     struct hostent *__restrict __result_buf,
                                     char *__restrict __buf, size_t __buflen,
                                     struct hostent **__restrict __result,
                                     int *__restrict __h_errnop);

typedef res_state (*__res_state_pfn_t)();
typedef int (*__poll_pfn_t)(struct pollfd fds[], nfds_t nfds, int timeout);

struct co_hook_t {
  socket_pfn_t socket_func;
  connect_pfn_t connect_func;
  close_pfn_t close_func;

  read_pfn_t read_func;
  write_pfn_t write_func;

  sendto_pfn_t sendto_func;
  recvfrom_pfn_t recvfrom_func;

  send_pfn_t send_func;
  recv_pfn_t recv_func;

  poll_pfn_t poll_func;

  setsockopt_pfn_t setsockopt_func;
  fcntl_pfn_t fcntl_func;

  setenv_pfn_t setenv_func;
  unsetenv_pfn_t unsetenv_func;
  getenv_pfn_t getenv_func;

  __res_state_pfn_t __res_state_func;

  gethostbyname_pfn_t gethostbyname_func;

  __poll_pfn_t __poll_func;
};
void co_replace_hook_func(co_hook_t *in, co_hook_t *sys);
int co_accept(int fd, struct sockaddr *addr, socklen_t *len);
