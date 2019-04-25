#include "aes.h"
#include "coder.h"
#include "printf.h"
#include "openssl/evp.h"
#include "openssl/err.h"

int aes_cbc_encrypt(const std::string& key, unsigned char* iv, const std::string& msg, int bit_type, int sign_type, std::string& encrypt_msg)
{
	ERR_load_crypto_strings();

	if (bit_type < AES_BIT_128 || bit_type > AES_BIT_256)
	{
		PRINTF_ERROR("invalid bit type: %s", std::to_string(bit_type).c_str());
		return AES_PARAM_TYPE_ERROR;
	}

	if (sign_type < AES_SIGN_CODE_BASE64 || sign_type > AES_SIGN_CODE_HEX)
	{
		PRINTF_ERROR("invalid sign type: %s", std::to_string(sign_type).c_str());
        return AES_PARAM_TYPE_ERROR;
	}

	EVP_CIPHER_CTX* ctx;
	ctx = EVP_CIPHER_CTX_new();

	const EVP_CIPHER* cipher;
	if (bit_type == AES_BIT_128)
	{
		cipher = EVP_aes_128_cbc();
	}
	else if (bit_type == AES_BIT_192)
	{
		cipher = EVP_aes_192_cbc();
	}
	else
	{
		cipher = EVP_aes_256_cbc();
	}

	unsigned char outbuf[msg.size() + EVP_CIPHER_block_size(cipher) + 1024];
	int outlen, tmplen;

	int rc = EVP_EncryptInit_ex(ctx, cipher, NULL, (const unsigned char*)key.c_str(), iv);
	if (rc != 1)
	{
		PRINTF_ERROR();
		EVP_CIPHER_CTX_free(ctx);
		return AES_CIPHER_INIT_ERROR;
	}

	rc = EVP_EncryptUpdate(ctx, outbuf, &outlen, (unsigned char*)msg.c_str(), msg.length());
	if (rc != 1)
	{
		PRINTF_ERROR();
		EVP_CIPHER_CTX_free(ctx);
		return AES_CIPHER_UPDATE_ERROR;
	}

	rc = EVP_EncryptFinal_ex(ctx, outbuf + outlen, &tmplen);
	if (rc != 1)
	{
		PRINTF_ERROR();
		EVP_CIPHER_CTX_free(ctx);
		return AES_CIPHER_FINAL_ERROR;
	}

	EVP_CIPHER_CTX_free(ctx);

	outlen += tmplen;

	if (sign_type == AES_SIGN_CODE_BASE64) 
	{
		std::string tmp((char*)outbuf, outlen);
		if (base64_encode(tmp, encrypt_msg) !=0)
		{
			PRINTF_ERROR("base64 encode sign error");
			return AES_PARAM_TYPE_ERROR;
		}
	}
	else
	{
		std::string tmp((char*)outbuf, outlen);
        encode_hex_string(tmp, encrypt_msg);
	}

	return 0;
}

int aes_cbc_decrypt(const std::string& key, unsigned char* iv, const std::string& encrypt_msg, int  bit_type, int sign_type, std::string& decrypt_msg)
{
	ERR_load_crypto_strings();

	if (bit_type < AES_BIT_128 || bit_type > AES_BIT_256)
	{
		PRINTF_ERROR("invalid bit type: %s", std::to_string(bit_type).c_str());
		return AES_PARAM_TYPE_ERROR;
	}

	if (sign_type < AES_SIGN_CODE_BASE64 || sign_type > AES_SIGN_CODE_HEX)
	{
		PRINTF_ERROR("invalid sign type: %s", std::to_string(sign_type).c_str());
        return AES_PARAM_TYPE_ERROR;
	}

	std::string decode_encrypt_msg;
	if (sign_type == AES_SIGN_CODE_BASE64)
	{
		if (base64_decode(encrypt_msg, decode_encrypt_msg) != 0)
		{
			PRINTF_ERROR("base64 decode sign error");
            return AES_PARAM_TYPE_ERROR;
        } 
	}
	else
	{
		buff_to_hex_string(encrypt_msg, decode_encrypt_msg);
	}

	EVP_CIPHER_CTX *ctx;
	ctx = EVP_CIPHER_CTX_new();

	const EVP_CIPHER* cipher;
	if (bit_type == AES_BIT_128)
	{
		cipher = EVP_aes_128_cbc();
	}
	else if (bit_type == AES_BIT_192)
	{
		cipher = EVP_aes_192_cbc();
	}
	else
	{
		cipher = EVP_aes_256_cbc();
	}

	unsigned char outbuf[decode_encrypt_msg.size() + EVP_CIPHER_block_size(cipher) + 1024];
	int outlen, tmplen;

	int rc = EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, (const unsigned char*)key.c_str(), iv);
	if (rc != 1)
	{
		PRINTF_ERROR();
		EVP_CIPHER_CTX_free(ctx);
		return AES_CIPHER_INIT_ERROR;
	}

	rc = EVP_DecryptUpdate(ctx, outbuf, &outlen, (const unsigned char*)decode_encrypt_msg.c_str(), decode_encrypt_msg.length());
	if (rc != 1)
	{
		PRINTF_ERROR();
		EVP_CIPHER_CTX_free(ctx);
		return AES_CIPHER_UPDATE_ERROR;
	}

	rc = EVP_DecryptFinal_ex(ctx, outbuf + outlen, &tmplen);
	if (rc != 1)
	{
		PRINTF_ERROR();
		EVP_CIPHER_CTX_free(ctx);
		return AES_CIPHER_FINAL_ERROR;
	}

	EVP_CIPHER_CTX_free(ctx);

	outlen += tmplen;

	decrypt_msg = std::string((char*)outbuf, outlen);

	return 0;
}