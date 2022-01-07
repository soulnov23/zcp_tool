#include <fcntl.h>

#include <coroutine>

#include "src/base/fd_guard.h"
#include "src/base/fd_lock_guard.h"
#include "src/base/lockfree_queue.h"
#include "src/base/printf_util.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/yaml.h"

void get_type(YAML::Node& node) {
    if (node.Type() == YAML::NodeType::Undefined) {
        PRINTF_DEBUG("YAML::NodeType::Undefined");
    } else if (node.Type() == YAML::NodeType::Null) {
        PRINTF_DEBUG("YAML::NodeType::Null");
    } else if (node.Type() == YAML::NodeType::Scalar) {
        PRINTF_DEBUG("YAML::NodeType::Scalar");
    } else if (node.Type() == YAML::NodeType::Sequence) {
        PRINTF_DEBUG("YAML::NodeType::Sequence");
    } else if (node.Type() == YAML::NodeType::Map) {
        PRINTF_DEBUG("YAML::NodeType::Map");
    } else {
        PRINTF_DEBUG("error");
    }
}

int lock(int fd_) {
    struct flock fd_lock;
    fd_lock.l_type = F_WRLCK;
    fd_lock.l_whence = SEEK_SET;
    fd_lock.l_start = 0;
    fd_lock.l_len = 0;
    if (-1 == fcntl(fd_, F_SETLK, &fd_lock)) {
        if (errno == EACCES || errno == EAGAIN) {
            PRINTF_ERROR("fd lock by other process");
            return -1;
        }
        PRINTF_ERROR("fd lock err");
        return -1;
    }
    return 0;
}

int unlock(int fd_) {
    struct flock fd_lock;
    fd_lock.l_type = F_UNLCK;
    fd_lock.l_whence = SEEK_SET;
    fd_lock.l_start = 0;
    fd_lock.l_len = 0;
    if (-1 == fcntl(fd_, F_SETLK, &fd_lock)) {
        PRINTF_ERROR("fd unlock err");
    }
    return 0;
}

int main(int argc, char* argv[]) {
    /*
    YAML::Node node = YAML::LoadFile("./test.yaml");
    if (node.IsNull()) {
        PRINTF_DEBUG("error");
        return -1;
    }
    get_type(node);
    YAML::Node matrix_node = node["matrix"];
    get_type(matrix_node);
    YAML::Node include = node["matrix"]["include"];
    get_type(include);
    YAML::Node env = node["matrix"]["env"];
    get_type(env);
    */
    fd_guard fd(open("test.txt", O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, S_IRWXU));
    if (fd == -1) {
        PRINTF_ERROR("open err");
        return -1;
    }
    unlock(fd);
    if (lock(fd) == -1) {
        PRINTF_ERROR("fd_lock.lock err");
        return -1;
    }
    if (lock(fd) == -1) {
        PRINTF_ERROR("fd_lock.lock err");
        return -1;
    }
    ssize_t size = write(fd, "123", 3);
    if (size != 3) {
        PRINTF_ERROR("write err");
        return -1;
    }
    unlock(fd);

    return 0;
}