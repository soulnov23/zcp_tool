#include "src/app/app.h"

int main(int argc, char* argv[]) { return app::get_instance_atomic()->start(argc, argv); }