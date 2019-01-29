#ifndef __CO_AIO_H__
#define __CO_AIO_H__
#include <unistd.h>

int co_aio_init_ct();
int co_aio_tick();

ssize_t co_pread(int fd, void *buf, size_t count, off_t offset);
ssize_t co_pwrite(int fd, void *buf, size_t count, off_t offset);

void co_init_pread2();
void co_force_pread2();

ssize_t co_pread2(int fd, void *buf, size_t count, off_t offset);
// int co_fsync( int fildes );

#endif
