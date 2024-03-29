#include "src/base/module.h"

#include <dlfcn.h>

#include "src/base/log.h"

/*
// 函数类型，函数类型在参数列表中会退化成函数指针
using FUNC = int(int);
// 函数指针
using FUNC = int(*)(int);
*/

int get_lib_func(const char* file_path, const char* func_name) {
    void* handle = dlopen(file_path, RTLD_LAZY);
    if (!handle) {
        LOG_SYSTEM_ERROR("dlopen");
        return -1;
    }
    dlerror();
    int (*func)(int);
    *(void**)(&func) = dlsym(handle, func_name);
    if (dlerror() != nullptr) {
        LOG_SYSTEM_ERROR("dlsym");
        dlclose(handle);
        return -1;
    }
    dlclose(handle);
    return 0;
}