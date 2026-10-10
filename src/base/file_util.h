#pragma once

#include <sys/types.h>
#include <sys/uio.h>

#include <string>

off_t get_file_size(const char* file_path);
std::string load_file_data(const char* file_path);