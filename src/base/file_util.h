#pragma once

#include <string>

std::string file_to_string(const char* file_path);
int string_to_file(const char* file_path, const std::string& data);