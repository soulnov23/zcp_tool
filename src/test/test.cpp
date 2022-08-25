#include <string>

#include "src/base/log.h"

std::string hash_game_id(std::string& uin) {
    unsigned num = 0;
    unsigned magic = 31;
    const unsigned char* p = (const unsigned char*)uin.c_str();

    for (; *p != '\0'; ++p) {
        num = num * magic + *p;
    }

    return std::to_string(num);
}

int main(int argc, char* argv[]) {
    std::string uin = "56056367093777704";
    std::string result = hash_game_id(uin);
    int verify_num = std::stol(result, nullptr, 10) % 10000;
    LOG_DEBUG("verify_num: {}", verify_num);
    uin = "10020";
    result = hash_game_id(uin);
    verify_num = std::stol(result, nullptr, 10) % 10000;
    LOG_DEBUG("verify_num: {}", verify_num);
    return 0;
}