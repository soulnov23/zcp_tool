#include "module.h"
#include <dlfcn.h>
#include "printf_utils.h"

typedef int (*FUNC)(int);

int get_lib_func(const char* file_path, const char* func_name) {
	void* handle = dlopen(file_path, RTLD_LAZY); 
	if (!handle) {
		PRINTF_ERROR();
		return -1;
	}
	dlerror();
	int (*func)(int);
	*(void**)(&func) = dlsym(handle, func_name);
	if (dlerror() != NULL) {
		PRINTF_ERROR();
		dlclose(handle);
		return -1;
	}
	dlclose(handle);
	return 0;
}