#include <fcntl.h>

#include <coroutine>

#include "src/base/fd_guard.h"
#include "src/base/fd_lock_guard.h"
#include "src/base/lockfree_queue.h"
#include "src/base/log.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/yaml.h"

void get_type(YAML::Node& node) {
    if (node.Type() == YAML::NodeType::Undefined) {
        LOG_DEBUG("YAML::NodeType::Undefined");
    } else if (node.Type() == YAML::NodeType::Null) {
        LOG_DEBUG("YAML::NodeType::Null");
    } else if (node.Type() == YAML::NodeType::Scalar) {
        LOG_DEBUG("YAML::NodeType::Scalar");
    } else if (node.Type() == YAML::NodeType::Sequence) {
        LOG_DEBUG("YAML::NodeType::Sequence");
    } else if (node.Type() == YAML::NodeType::Map) {
        LOG_DEBUG("YAML::NodeType::Map");
    } else {
        LOG_DEBUG("error");
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
            LOG_ERROR("fd lock by other process");
            return -1;
        }
        LOG_SYSTEM_ERROR("fcntl fd: {}", fd_);
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
        LOG_SYSTEM_ERROR("fcntl fd: {}", fd_);
        return -1;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    /*
    YAML::Node node = YAML::LoadFile("./test.yaml");
    if (node.IsNull()) {
        LOG_DEBUG("error");
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
    fd_guard fd1(open("test.txt", O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, S_IRWXU));
    if (fd1 == -1) {
        LOG_ERROR("open err");
        return -1;
    }
    unlock(fd1);
    if (lock(fd1) == -1) {
        LOG_ERROR("fd_lock.lock err");
        return -1;
    }
    if (lock(fd1) == -1) {
        LOG_ERROR("fd_lock.lock err");
        return -1;
    }
    ssize_t size = write(fd1, "123", 3);
    if (size != 3) {
        LOG_ERROR("write err");
        return -1;
    }
    while (true) {
        sleep(3);
    }

    return 0;
}