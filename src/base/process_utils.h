#pragma once

int get_pid_file(const char* file_path);
int set_pid_file(const char* file_path);
int single_process(const char* proc_name);
int daemon_process();