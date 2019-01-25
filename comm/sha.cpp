#include "sha.h"
#include "openssl/evp.h"
#include "openssl/sha.h"

int sha256(const std::string &data_in, std::string &data_out)
{
    char buf[2];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data_in.c_str(), data_in.size());
    SHA256_Final(hash, &sha256);
    std::string new_string = "";
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(buf,"%02x",hash[i]);
        new_string = new_string + buf;
    }
    data_out = new_string;

    return 0;
}