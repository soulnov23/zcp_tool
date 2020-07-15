#ifndef __RSA_H__
#define __RSA_H__

#include <string>

#define COMM_MAX_BUFLEN 2048

#define RSA_PARAM_TYPE_ERROR -1  //参数错误
#define RSA_KEY_ERROR        -2  // Key错误
#define RSA_VERIFY_ERROR     -3  //签名验证错误
#define RSA_DECRYPT_ERROR    -4  //解密失败
#define RSA_SIGN_ERROR       -5  //签名失败

#define TOOLS_RSA_NO_PADDING         1
#define TOOLS_RSA_PKCS1_PADDING      2
#define TOOLS_RSA_PKCS1_OAEP_PADDING 3

/*!
 * \enum	digest_algorithm
 * \brief	摘要算法
 */
enum digest_algorithm {
    DIGEST_SHA1   = 0,  //!< SHA-1摘要算法
    DIGEST_SHA256 = 1
};
enum rsa_cipher_type { CIPHER_TYPE_BASE64 = 0, CIPHER_TYPE_HEX = 1 };
/*!
 * \enum	sign_code_type
 * \brief	签名编码类型
 */
enum sign_code_type {
    SIGN_CODE_RAW    = 0,  //!< 签名未编码
    SIGN_CODE_BASE64 = 1,  //!< 签名经过Base64编码
    SIGN_CODE_HEX    = 2   //!< 签名经过Hex编码
};

/*!
 * \fn int verify_rsa_sign(const std::string& data_in, const std::string& sign,
 *const std::string& public_key, int sign_type, int digest_algo = DIGEST_SHA1);
 * \brief	验证RSA签名
 *
 * 可用\a upay::get_last_error_info()
 * \param	data_in	   	原始数据
 * \param	sign	   	待验证签名
 * \param	public_key 	RSA公钥
 * \param	sign_type  	\a sign 编码类型, 见\a
 *upay::crypto::sign_code_type
 * \param	digest_algo	(Optional) 摘要算法, 见\a
 *upay::crypto::digest_algorithm
 * \return	0为成功,其余错误见\a RSA_PARAM_TYPE_ERROR , \a RSA_KEY_ERROR, \a
 *RSA_VERIFY_ERROR
 */
int verify_rsa_sign(const std::string& data_in, const std::string& sign, const std::string& public_key, int sign_type,
                    int digest_algo = DIGEST_SHA1);

/*!
 * \fn int verify_rsa_sign(const std::string& data_in, const std::string& sign,
 *const std::string& public_key, int sign_type, int digest_algo = DIGEST_SHA1);
 * \brief	验证RSA SHA256签名
 *
 * 可用\a upay::get_last_error_info()
 * \param	data_in	   	原始数据
 * \param	sign	   	待验证签名
 * \param	public_key 	RSA公钥
 * \param	sign_type  	\a sign 编码类型, 见\a
 *upay::crypto::sign_code_type
 * \param	digest_algo	(Optional) 摘要算法, 见\a
 *upay::crypto::digest_algorithm
 * \return	0为成功,其余错误见\a RSA_PARAM_TYPE_ERROR , \a RSA_KEY_ERROR, \a
 *RSA_VERIFY_ERROR
 */
int verify_rsa2_sign(const std::string& data_in, const std::string& sign, const std::string& public_key, int sign_type,
                     int digest_algo = DIGEST_SHA256);

int rsa_pubkey_decrypt(const std::string& cipher_in,  //密文
                       int cipher_type_in,            //密文类型
                       // base64(CIPHER_TYPE_BASE64)
                       // hex(CIPHER_TYPE_HEX)
                       int padding_mode_in,            // TOOLS_RSA_NO_PADDING
                                                       // TOOLS_RSA_PKCS1_PADDING
                                                       // TOOLS_RSA_PKCS1_OAEP_PADDING
                       const std::string& pub_key_in,  //公钥
                       std::string& clear_data_out);   //明文

/*!
 * \fn int calculate_rsa_sign(const std::string& data_in, const std::string&
 * private_key, int sign_type, std::string& sign)
 * \brief  用RSA算法用私钥对数据进行签名
 * 可用\a upay::get_last_error_info()
 * \param	data_in	   	原始数据
 * \param	private_key 	RSA私钥
 * \param	sign_type  	\a sign 编码类型, 见\a
 * upay::crypto::sign_code_type
 * \param   sign   对数据进行签名后的结果
 * \return	0为成功,其余错误见\a RSA_PARAM_TYPE_ERROR , \a RSA_KEY_ERROR,
 */
int calculate_rsa_sign(const std::string& data_in, const std::string& private_key, int sign_type, std::string& sign);

/*!
 * \fn int calculate_rsa_sign(const std::string& data_in, const std::string&
 * private_key, int sign_type, std::string& sign)
 * \brief  用RSA2算法用私钥对数据进行签名
 * 可用\a upay::get_last_error_info()
 * \param	data_in	   	原始数据
 * \param	private_key 	RSA私钥
 * \param	sign_type  	\a sign 编码类型, 见\a
 * upay::crypto::sign_code_type
 * \param   sign   对数据进行签名后的结果
 * \return	0为成功,其余错误见\a RSA_PARAM_TYPE_ERROR , \a RSA_KEY_ERROR,
 */
int calculate_rsa2_sign(const std::string& data_in, const std::string& private_key, int sign_type, std::string& sign);

#endif
