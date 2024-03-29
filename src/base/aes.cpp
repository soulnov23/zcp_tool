#include "src/base/aes.h"

#include <openssl/err.h>
#include <openssl/evp.h>

#include "src/base/base64.h"
#include "src/base/coder.h"
#include "src/base/log.h"

#define SET_SSL_ERROR() LOG_ERROR("errno:{} {}", ERR_get_error(), ERR_error_string(ERR_get_error(), nullptr))

int aes_cbc_encrypt(const std::string& key, unsigned char* iv, const std::string& msg, int bit_type, int sign_type,
                    std::string& encrypt_msg) {
    ERR_load_crypto_strings();

    if (bit_type < AES_BIT_128 || bit_type > AES_BIT_256) {
        LOG_ERROR("invalid bit type: {}", std::to_string(bit_type).c_str());
        return AES_PARAM_TYPE_ERROR;
    }

    if (sign_type < AES_SIGN_CODE_BASE64 || sign_type > AES_SIGN_CODE_HEX) {
        LOG_ERROR("invalid sign type: {}", std::to_string(sign_type).c_str());
        return AES_PARAM_TYPE_ERROR;
    }

    EVP_CIPHER_CTX* ctx;
    ctx = EVP_CIPHER_CTX_new();

    const EVP_CIPHER* cipher;
    if (bit_type == AES_BIT_128) {
        cipher = EVP_aes_128_cbc();
    } else if (bit_type == AES_BIT_192) {
        cipher = EVP_aes_192_cbc();
    } else {
        cipher = EVP_aes_256_cbc();
    }

    int key_len = EVP_CIPHER_key_length(cipher);
    if (key_len != int(key.length())) {
        LOG_ERROR("key length {} not equal EVP_CIPHER_key_length {}", key.length(), key_len);
        return AES_KEY_LENGTH_ERROR;
    }
    int iv_len = EVP_CIPHER_iv_length(cipher);
    if (iv_len != int(strlen((const char*)iv))) {
        LOG_ERROR("iv length {} not equal EVP_CIPHER_iv_length {}", strlen((const char*)iv), iv_len);
        return AES_IV_LENGTH_ERROR;
    }

    unsigned char outbuf[msg.size() + EVP_CIPHER_block_size(cipher) + 1024];
    int outlen, tmplen;

    int rc = EVP_EncryptInit_ex(ctx, cipher, nullptr, (const unsigned char*)key.c_str(), iv);
    if (rc != 1) {
        SET_SSL_ERROR();
        EVP_CIPHER_CTX_free(ctx);
        return AES_CIPHER_INIT_ERROR;
    }

    rc = EVP_EncryptUpdate(ctx, outbuf, &outlen, (unsigned char*)msg.c_str(), msg.length());
    if (rc != 1) {
        SET_SSL_ERROR();
        EVP_CIPHER_CTX_free(ctx);
        return AES_CIPHER_UPDATE_ERROR;
    }

    rc = EVP_EncryptFinal_ex(ctx, outbuf + outlen, &tmplen);
    if (rc != 1) {
        SET_SSL_ERROR();
        EVP_CIPHER_CTX_free(ctx);
        return AES_CIPHER_FINAL_ERROR;
    }

    EVP_CIPHER_CTX_free(ctx);

    outlen += tmplen;

    if (sign_type == AES_SIGN_CODE_BASE64) {
        std::string tmp((char*)outbuf, outlen);
        base64_encode(tmp, encrypt_msg);
    } else {
        std::string tmp((char*)outbuf, outlen);
        encode_hex_string(tmp, encrypt_msg);
    }

    return 0;
}

int aes_cbc_decrypt(const std::string& key, unsigned char* iv, const std::string& encrypt_msg, int bit_type, int sign_type,
                    std::string& decrypt_msg) {
    ERR_load_crypto_strings();

    if (bit_type < AES_BIT_128 || bit_type > AES_BIT_256) {
        LOG_ERROR("invalid bit type: {}", std::to_string(bit_type).c_str());
        return AES_PARAM_TYPE_ERROR;
    }

    if (sign_type < AES_SIGN_CODE_BASE64 || sign_type > AES_SIGN_CODE_HEX) {
        LOG_ERROR("invalid sign type: {}", std::to_string(sign_type).c_str());
        return AES_PARAM_TYPE_ERROR;
    }

    std::string decode_encrypt_msg;
    if (sign_type == AES_SIGN_CODE_BASE64) {
        if (base64_decode(encrypt_msg, decode_encrypt_msg) != 0) {
            LOG_ERROR("base64 decode sign error");
            return AES_PARAM_TYPE_ERROR;
        }
    } else {
        buff_to_hex_string(encrypt_msg, decode_encrypt_msg);
    }

    EVP_CIPHER_CTX* ctx;
    ctx = EVP_CIPHER_CTX_new();

    const EVP_CIPHER* cipher;
    if (bit_type == AES_BIT_128) {
        cipher = EVP_aes_128_cbc();
    } else if (bit_type == AES_BIT_192) {
        cipher = EVP_aes_192_cbc();
    } else {
        cipher = EVP_aes_256_cbc();
    }

    int key_len = EVP_CIPHER_key_length(cipher);
    if (key_len != int(key.length())) {
        LOG_ERROR("key length {} not equal EVP_CIPHER_key_length {}", key.length(), key_len);
        return AES_KEY_LENGTH_ERROR;
    }
    int iv_len = EVP_CIPHER_iv_length(cipher);
    if (iv_len != int(strlen((const char*)iv))) {
        LOG_ERROR("iv length {} not equal EVP_CIPHER_iv_length {}", strlen((const char*)iv), iv_len);
        return AES_IV_LENGTH_ERROR;
    }

    unsigned char outbuf[decode_encrypt_msg.size() + EVP_CIPHER_block_size(cipher) + 1024];
    int outlen, tmplen;

    int rc = EVP_DecryptInit_ex(ctx, cipher, nullptr, (const unsigned char*)key.c_str(), iv);
    if (rc != 1) {
        SET_SSL_ERROR();
        EVP_CIPHER_CTX_free(ctx);
        return AES_CIPHER_INIT_ERROR;
    }

    rc = EVP_DecryptUpdate(ctx, outbuf, &outlen, (const unsigned char*)decode_encrypt_msg.c_str(), decode_encrypt_msg.length());
    if (rc != 1) {
        SET_SSL_ERROR();
        EVP_CIPHER_CTX_free(ctx);
        return AES_CIPHER_UPDATE_ERROR;
    }

    rc = EVP_DecryptFinal_ex(ctx, outbuf + outlen, &tmplen);
    if (rc != 1) {
        SET_SSL_ERROR();
        EVP_CIPHER_CTX_free(ctx);
        return AES_CIPHER_FINAL_ERROR;
    }

    EVP_CIPHER_CTX_free(ctx);

    outlen += tmplen;

    decrypt_msg = std::string((char*)outbuf, outlen);

    return 0;
}