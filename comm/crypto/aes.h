#ifndef __AES_H__
#define __AES_H__

#include <string>

#define AES_PARAM_TYPE_ERROR -1
#define AES_CIPHER_INIT_ERROR -2
#define AES_CIPHER_UPDATE_ERROR -3
#define AES_CIPHER_FINAL_ERROR -4
#define AES_KEY_LENGTH_ERROR -5
#define AES_IV_LENGTH_ERROR -6

enum AES_SIGN_TYPE {
    AES_SIGN_CODE_BASE64 = 0,  //!< 签名经过Base64编码
    AES_SIGN_CODE_HEX = 1,     //!< 签名经过Hex编码
};

enum AES_BIT_TYPE {
    AES_BIT_128 = 0,
    AES_BIT_192 = 1,
    AES_BIT_256 = 2,
};

// 返回值不为0则表示错误，通过upay::get_last_error_info()获取错误信息
int aes_cbc_encrypt(const std::string& key, unsigned char* iv,
                    const std::string& msg, int bit_type, int sign_type,
                    std::string& encrypt_msg);
int aes_cbc_decrypt(const std::string& key, unsigned char* iv,
                    const std::string& encrypt_msg, int bit_type, int sign_type,
                    std::string& decrypt_msg);

#endif