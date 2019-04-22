#ifndef __SHA_H__
#define __SHA_H__

#include <string>

int sha256(const std::string &data_in, std::string &data_out);
std::string hmac_sha1_base64(const std::string &key, const std::string &data);
std::string hmac_sha1_hex(const std::string &key, const std::string &data);
std::string hmac_md5_hex(const std::string &key, const std::string &data);
std::string hmac_sha256_base64(const std::string &key, const std::string &data);
std::string hmac_sha256_hex(const std::string &key, const std::string &data);

#endif 