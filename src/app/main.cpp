#include "src/app/app.h"
#include "src/base/log.h"

int main(int argc, char* argv[]) {
    app* instance = app::get_instance_atomic();
    if (instance == nullptr) {
        CONSOLE_ERROR("single instance err");
        return -1;
    }
    return instance->start(argc, argv);
}