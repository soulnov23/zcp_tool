#pragma once

#include <string>

/*
uuid_generate_random使用高质量的随机数生成器/dev/urandom或者/dev/random
uuid_generate_time使用当前时间和本地以太网mac地址
uuid_generate根据/dev/urandom或者/dev/random是否可用，可用调用uuid_generate_random，不可用就调用uuid_generate_time
*/
std::string uuid_generate();