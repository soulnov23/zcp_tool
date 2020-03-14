#include "sha.h"
#include "coder.h"
#include "base64.h"
#include "openssl/rsa.h"
#include "openssl/sha.h"
#include "openssl/hmac.h"
#include "openssl/pem.h"
#include "openssl/bio.h"
#include <string.h>

int sha256(const std::string& data_in, std::string& data_out) {
    char buf[3];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data_in.c_str(), data_in.size());
    SHA256_Final(hash, &sha256);
    std::string new_string = "";
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(buf, "%02x", hash[i]);
        new_string = new_string + buf;
    }
    data_out = new_string;

    return 0;
}

std::string hmac_sha1_base64(const std::string& key, const std::string& data) {

    unsigned char* digest;
    unsigned int len = 0;

    // You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512,
    // etc
    digest = HMAC(EVP_sha1(), (unsigned char*)key.c_str(), key.length(),
                  (unsigned char*)data.c_str(), data.length(), nullptr, &len);

    // Be careful of the length of string with the choosen hash engine. SHA1
    // produces a 20-byte hash value which rendered as 40 characters.
    // Change the length accordingly with your choosen hash engine

    std::string str_out;
    // string str_in = (char* )digest;
    //上面的写法是有问题的，会导致二进制串里面有\0被截断
    std::string str_in((const char*)digest, len);
    base64_encode(str_in, str_out);
    return str_out;
}

std::string hmac_sha256_hex(const std::string& key, const std::string& data) {

    unsigned char* digest;
    unsigned int len = 0;

    // You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512,
    // etc
    digest = HMAC(EVP_sha256(), (unsigned char*)key.c_str(), key.length(),
                  (unsigned char*)data.c_str(), data.length(), nullptr, &len);

    // Be careful of the length of string with the choosen hash engine. SHA1
    // produces a 20-byte hash value which rendered as 40 characters.
    // Change the length accordingly with your choosen hash engine

    std::string str_out;
    // string str_in = (char* )digest;

    // str2hex
    for (unsigned int i = 0; i < len; ++i) {
        char szTmp[3] = {0};
        snprintf(szTmp, sizeof(szTmp), "%02x", digest[i]);
        str_out += szTmp;
    }

    return str_out;
}

std::string hmac_sha256_base64(const std::string& key,
                               const std::string& data) {

    unsigned char* digest;
    unsigned int len = 0;

    // You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512,
    // etc
    digest = HMAC(EVP_sha256(), (unsigned char*)key.c_str(), key.length(),
                  (unsigned char*)data.c_str(), data.length(), nullptr, &len);

    // Be careful of the length of string with the choosen hash engine. SHA1
    // produces a 20-byte hash value which rendered as 40 characters.
    // Change the length accordingly with your choosen hash engine

    std::string str_out;
    // string str_in = (char* )digest;
    //上面的写法是有问题的，会导致二进制串里面有\0被截断
    std::string str_in((const char*)digest, len);
    base64_encode(str_in, str_out);
    return str_out;
}

std::string hmac_sha1_hex(const std::string& key, const std::string& data) {

    unsigned char* digest;

    // You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512,
    // etc
    digest =
        HMAC(EVP_sha1(), (unsigned char*)key.c_str(), key.length(),
             (unsigned char*)data.c_str(), data.length(), nullptr, nullptr);

    // Be careful of the length of string with the choosen hash engine. SHA1
    // produces a 20-byte hash value which rendered as 40 characters.
    // Change the length accordingly with your choosen hash engine

    char szHexHMACSha1[41];
    for (int i = 0; i < 20; ++i) {
        sprintf(&szHexHMACSha1[i * 2], "%02x", (unsigned int)digest[i]);
    }
    szHexHMACSha1[40] = '\0';

    return szHexHMACSha1;
}

std::string hmac_md5_hex(const std::string& key, const std::string& data) {
    unsigned char* digest;

    // You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512,
    // etc
    digest =
        HMAC(EVP_md5(), (unsigned char*)key.c_str(), key.length(),
             (unsigned char*)data.c_str(), data.length(), nullptr, nullptr);

    // Be careful of the length of string with the choosen hash engine. SHA1
    // produces a 20-byte hash value which rendered as 40 characters.
    // Change the length accordingly with your choosen hash engine

    char szHexHMACSha1[34];
    for (int i = 0; i < 16; ++i) {
        sprintf(&szHexHMACSha1[i * 2], "%02x", (unsigned int)digest[i]);
    }
    szHexHMACSha1[33] = '\0';

    return szHexHMACSha1;
}

std::string sha1_hex(const std::string& data) {
    // SHA1 res is placed in a static array, it's not thread safe
    unsigned char* digest =
        SHA1((unsigned char*)data.c_str(), data.length(), nullptr);

    std::string str_out;
    for (unsigned int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        char szTmp[3] = {0};
        snprintf(szTmp, sizeof(szTmp), "%02x", digest[i]);
        str_out += szTmp;
    }

    return str_out;
}

std::string sha256_hex(const std::string& data) {
    // SHA256 res is placed in a static array, it's not thread safe
    unsigned char* digest =
        SHA256((unsigned char*)data.c_str(), data.length(), nullptr);

    std::string str_out;
    for (unsigned int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        char szTmp[3] = {0};
        snprintf(szTmp, sizeof(szTmp), "%02x", digest[i]);
        str_out += szTmp;
    }

    return str_out;
}

std::string sha256_rsa_base64(const std::string& key, const std::string& data) {
    std::string format_public_key = "-----BEGIN PRIVATE KEY-----\n";
    for (std::string::size_type i = 0; i < key.length(); i += 64) {
        format_public_key += key.substr(i, 64);
        format_public_key += '\n';
    }
    format_public_key += "-----END PRIVATE KEY-----\n";

    //从字符串读取RSA私钥
    BIO* bio = nullptr;
    if ((bio = BIO_new_mem_buf((void*)(format_public_key.c_str()),
                               format_public_key.length())) == nullptr) {
        return "";
    }

    //从bio结构中得到RSA结构
    RSA* rsa = nullptr;
    if ((rsa = PEM_read_bio_RSAPrivateKey(bio, nullptr, nullptr, nullptr)) ==
        nullptr) {
        BIO_free(bio);
        return "";
    }

    // SHA256 res is placed in a static array, it's not thread safe
    unsigned char* digest =
        SHA256((unsigned char*)data.c_str(), data.length(), nullptr);

    unsigned int siglen = RSA_size(rsa);
    unsigned char* sigret =
        (unsigned char*)malloc(sizeof(unsigned char) * siglen);
    memset(sigret, 0x0, sizeof(unsigned char) * siglen);
    if (RSA_sign(NID_sha256, digest, SHA256_DIGEST_LENGTH, sigret, &siglen,
                 rsa) != 1) {
        BIO_free(bio);
        RSA_free(rsa);
        free(sigret);
        return "";
    }

    std::string str_in((const char*)sigret, siglen);
    std::string str_out;
    base64_encode(str_in, str_out);

    BIO_free(bio);
    RSA_free(rsa);
    free(sigret);

    return str_out;
}