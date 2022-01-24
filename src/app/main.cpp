#include "src/app/app.h"
#include "src/base/log.h"

int main(int argc, char* argv[]) {
    logger* logger_instance = logger::get_instance_atomic();
    if (logger_instance == nullptr) {
        LOG_ERROR("logger single instance error");
        return -1;
    }
    app* app_instance = app::get_instance_atomic();
    if (app_instance == nullptr) {
        LOG_ERROR("app single instance error");
        return -1;
    }
    return app_instance->start(argc, argv);
}