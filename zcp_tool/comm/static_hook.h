#ifndef __STATIC_HOOK_H__
#define __STATIC_HOOK_H__

#include <sys/mman.h>
#include <sys/utsname.h>

/* 返回码
 *     0: hook成功
 *     1: 不支持的系统架构
 *     2: 内存权限修改失败
 * */
template <typename T>
int static_hook(T old_func, T new_func) {
    static const int JMP_SIZE = 5;
    union ConvT {
        unsigned char* ip;
        unsigned long val;
    };

    int ret = 0;

    struct utsname sysinfo = {};
    ::uname(&sysinfo);
    if (strcmp(sysinfo.machine, "x86_64") != 0) {
        return 1;
    }

    ConvT old_addr;
    old_addr.ip = reinterpret_cast<unsigned char*>(old_func);

    ConvT new_addr;
    new_addr.ip = reinterpret_cast<unsigned char*>(new_func);

    /* 解除内存保护，必须以页块的头为起始点 */
    unsigned long mask   = sysconf(_SC_PAGE_SIZE) - 1;
    unsigned long remain = old_addr.val & mask;
    unsigned long begin  = old_addr.val & ~mask;
    ret                  = mprotect(reinterpret_cast<void*>(begin), remain + JMP_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC);
    if (ret != 0) {
        return 2;
    }

    /* 目标函数注入jmp指令  */
    old_addr.ip[0] = 0xE9;

    /* 注入jmp参数，偏移是new-old，还要减去jmp指令长度5 */
    *reinterpret_cast<unsigned long*>(old_addr.ip + 1) = new_addr.val - (old_addr.val + JMP_SIZE);
    return 0;
}

#endif