#include "src/app/app.h"
#include "src/base/log.h"

int main(int argc, char* argv[]) {
    // instance()返回引用且保证构造完成，无需判空
    logger& logger_instance = logger::instance();
    int ret = app::instance().start(argc, argv);
    // logger采用no_destroy策略，退出前显式刷盘并回收异步线程
    logger_instance.shutdown();
    return ret;
}
