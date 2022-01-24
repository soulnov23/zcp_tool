#pragma once

#include <sys/types.h>
#include <sys/uio.h>

#include <string>

off_t get_file_size(const char* file_path);
std::string load_file_data(const char* file_path);
ssize_t read_file(int fd, void* data, size_t size, loff_t offset = 0);
ssize_t write_file(int fd, void* data, size_t size, loff_t offset = 0);
ssize_t readv_file(int fd, iovec* iov, int count, loff_t offset = 0);
ssize_t writev_file(int fd, iovec* iov, int count, loff_t offset = 0);