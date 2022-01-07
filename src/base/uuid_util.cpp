#include "src/base/uuid_util.h"

#include "uuid.h"

std::string uuid_generate() {
    uuid_t uuid;
    char temp_uuid[64];
    uuid_generate(uuid);
    uuid_unparse_lower(uuid, temp_uuid);
    std::string str_uuid(temp_uuid);
    return str_uuid;
}