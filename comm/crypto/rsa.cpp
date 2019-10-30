#include <string>
#include <string.h>
#include "openssl/rsa.h"
#include "openssl/pem.h"
#include "openssl/bio.h"
#include "openssl/evp.h"
#include "openssl/err.h"
#include "coder.h"
#include "base64.h"
#include "rsa.h"
#include "printf_utils.h"

#define SET_SSL_ERROR() { \
	PRINTF_ERROR("errno:%lu %s", ERR_get_error(), ERR_error_string(ERR_get_error(), NULL)) \
}

int hex_decode(std::string& src, std::string& dst) {
	dst = "";
	for (size_t i = 0; i < src.size(); i += 2) {
		int c = 0;
		sscanf(src.c_str() + i, "%2X", &c);
		dst += (char)c;   
	}

	return 0;
}

int verify_rsa_sign(const std::string& data_in, const std::string& sign, const std::string& public_key, int sign_type, int digest_algo) { 
    ERR_load_crypto_strings();

    int ret = 0;

	RSA *rsa = NULL;
	BIO *bio = NULL;
	EVP_PKEY *evp_pkey = NULL;
	unsigned char digest[SHA512_DIGEST_LENGTH];
    unsigned int digest_len = sizeof(digest);
	EVP_MD_CTX ctx;
    std::string format_public_key;
    std::string coded_sign = sign;

    if (digest_algo != DIGEST_SHA1) {
		PRINTF_ERROR("invalid digest algorithm: %s", std::to_string(digest_algo).c_str());
        ret = RSA_PARAM_TYPE_ERROR;
        goto ready_ret;
    }

    if (sign_type < SIGN_CODE_RAW || sign_type > SIGN_CODE_HEX) {
		PRINTF_ERROR("invalid sign type: %s", std::to_string(sign_type).c_str());
        ret = RSA_PARAM_TYPE_ERROR;
        goto ready_ret;
    }
    
    format_public_key =  "-----BEGIN PUBLIC KEY-----\n";
    for (std::string::size_type i = 0; i < public_key.length(); i += 64) {
        format_public_key += public_key.substr(i, 64);
        format_public_key += '\n';
    }
	format_public_key += "-----END PUBLIC KEY-----\n";

	if ((bio = BIO_new_mem_buf((void*)(format_public_key.c_str()), format_public_key.length())) == NULL)        {
        SET_SSL_ERROR();
		ret = RSA_KEY_ERROR;
        goto ready_ret;
	}

	rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);   
	if (NULL == rsa) {
        SET_SSL_ERROR();
		ret = RSA_KEY_ERROR;
        goto free_bio;
	}

	evp_pkey = EVP_PKEY_new();
	if (evp_pkey == NULL) {
        SET_SSL_ERROR();
		ret = RSA_KEY_ERROR;
        goto free_rsa;
	}

	if (EVP_PKEY_set1_RSA(evp_pkey, rsa) != 1) {
        SET_SSL_ERROR();
		ret = RSA_KEY_ERROR;
        goto free_evp_pkey;
	}

	EVP_MD_CTX_init(&ctx);
     //if (DIGEST_SHA1 == digest_algo) ...
	EVP_DigestInit(&ctx, EVP_sha1());
	EVP_DigestUpdate(&ctx, data_in.c_str(), data_in.length());
	EVP_DigestFinal(&ctx, digest, &digest_len);

    if (digest_len > SHA512_DIGEST_LENGTH) {
		PRINTF_ERROR("SHA1 digest length error");
        ret = RSA_PARAM_TYPE_ERROR;
        goto free_evp_pkey;
    }

    //if digest.length < digets_len ..

	if (sign_type == SIGN_CODE_BASE64) {
		if (base64_decode(sign, coded_sign) != 0){
		    PRINTF_ERROR("base64 decode sign error");
             ret = RSA_PARAM_TYPE_ERROR;
             goto free_evp_pkey;
         } 
	}
	else if (sign_type == SIGN_CODE_HEX) {
         buff_to_hex_string(sign, coded_sign);
	}
	
     //if algorithm == sha1
	if (RSA_verify(NID_sha1, digest, digest_len, (unsigned char* )coded_sign.c_str(), coded_sign.length(), rsa) != 1) {
         SET_SSL_ERROR();
		ret = RSA_VERIFY_ERROR;
	}

free_evp_pkey:
	EVP_PKEY_free(evp_pkey);
free_rsa:
	RSA_free(rsa);
free_bio:
	BIO_free(bio);
ready_ret:
	return ret;
}

int verify_rsa2_sign(const std::string& data_in, const std::string& sign, const std::string& public_key, int sign_type, int digest_algo) { 
    ERR_load_crypto_strings();

    int ret = 0;

	RSA *rsa = NULL;
	BIO *bio = NULL;
	EVP_PKEY *evp_pkey = NULL;
	unsigned char digest[SHA512_DIGEST_LENGTH];
    unsigned int digest_len = sizeof(digest);
	EVP_MD_CTX ctx;
    std::string format_public_key;
    std::string coded_sign = sign;

    if (digest_algo != DIGEST_SHA256) {
		PRINTF_ERROR("invalid digest algorithm: %s", std::to_string(digest_algo).c_str());
        ret = RSA_PARAM_TYPE_ERROR;
        goto ready_ret;
    }

    if (sign_type < SIGN_CODE_RAW || sign_type > SIGN_CODE_HEX) {
		PRINTF_ERROR("invalid sign type: %s", std::to_string(sign_type).c_str());
        ret = RSA_PARAM_TYPE_ERROR;
        goto ready_ret;
    }
   
    format_public_key =  "-----BEGIN PUBLIC KEY-----\n";
    for (std::string::size_type i = 0; i < public_key.length(); i += 64) {
        format_public_key += public_key.substr(i, 64);
        format_public_key += '\n';
    }
	format_public_key += "-----END PUBLIC KEY-----\n";

	if ((bio = BIO_new_mem_buf((void*)(format_public_key.c_str()), format_public_key.length())) == NULL)        {
        SET_SSL_ERROR();
		ret = RSA_KEY_ERROR;
        goto ready_ret;
	}

	rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);   
	if (NULL == rsa) {
        SET_SSL_ERROR();
		ret = RSA_KEY_ERROR;
        goto free_bio;
	}

	evp_pkey = EVP_PKEY_new();
	if (evp_pkey == NULL) {
        SET_SSL_ERROR();
		ret = RSA_KEY_ERROR;
        goto free_rsa;
	}

	if (EVP_PKEY_set1_RSA(evp_pkey, rsa) != 1) {
        SET_SSL_ERROR();
		ret = RSA_KEY_ERROR;
        goto free_evp_pkey;
	}

	EVP_MD_CTX_init(&ctx);
    //if (DIGEST_SHA1 == digest_algo) ...
	EVP_DigestInit(&ctx, EVP_sha256());
	EVP_DigestUpdate(&ctx, data_in.c_str(), data_in.length());
	EVP_DigestFinal(&ctx, digest, &digest_len);

    if (digest_len > SHA512_DIGEST_LENGTH) {
		PRINTF_ERROR("SHA1 digest length error");
        ret = RSA_PARAM_TYPE_ERROR;
        goto free_evp_pkey;
    }

    //if digest.length < digets_len ..

	if (sign_type == SIGN_CODE_BASE64) {
		if (base64_decode(sign, coded_sign) != 0){
		    PRINTF_ERROR("base64 decode sign error");
            ret = RSA_PARAM_TYPE_ERROR;
            goto free_evp_pkey;
        } 
	}
	else if (sign_type == SIGN_CODE_HEX) {
        buff_to_hex_string(sign, coded_sign);
	}
	
    //if algorithm == sha1
	if (RSA_verify(NID_sha256, digest, digest_len, (unsigned char* )coded_sign.c_str(), coded_sign.length(), rsa) != 1) {
        SET_SSL_ERROR();
		ret = RSA_VERIFY_ERROR;
	}

free_evp_pkey:
	EVP_PKEY_free(evp_pkey);
free_rsa:
	RSA_free(rsa);
free_bio:
	BIO_free(bio);
ready_ret:
	return ret;
}


int public_key_str2rsa(const std::string& public_key_in,
                            RSA* & rsa) {
	BIO *bio =  NULL;
	int ret;

    std::string pub_key = public_key_in;
	//int keyLen = strPubKey.size();
	for(std::string::size_type i = 64; i < pub_key.size(); i += 64) {
		if (pub_key[i] != '\n') {
			pub_key.insert(i, "\n");
		}
		++i;
	}
	pub_key.insert(0, "-----BEGIN PUBLIC KEY-----\n");
	pub_key.append("\n-----END PUBLIC KEY-----\n");
	char*  pub_key_tmp = const_cast<char* >(pub_key.c_str());

	// 从字符串读取RSA公钥
	if ((bio = BIO_new_mem_buf(pub_key_tmp, pub_key.length())) == NULL)        {
		char err_buf[512] = {0};
		ERR_error_string_n(ERR_get_error(), err_buf, sizeof(err_buf));
		PRINTF_ERROR("BIO_new_mem_buf err %s", err_buf);
        ret = RSA_KEY_ERROR;
		return ret;
	}

	// 从bio结构中得到RSA结构
	rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);   
	if (NULL == rsa) {
		char err_buf[512] = {0};
		ERR_error_string_n(ERR_get_error(), err_buf, sizeof(err_buf));
		PRINTF_ERROR("PEM_read_bio_RSA_PUBKEY err %s", err_buf);
        ret = RSA_KEY_ERROR;
		return ret;
	}
	BIO_free(bio);
    return 0;
}

int rsa_public_decrypt(RSA* rsa,
                            const std::string& cipher_data_in, 
                            int cipher_type, 
                            int padding_mode_in,
                            std::string& clear_data_out) {
    EVP_PKEY *pKey = NULL;
	int ret;

    // should include <openssl/evp.h>
    // private key allocation, EVP_PKEY_free() frees up the private key
    pKey = EVP_PKEY_new();
    if (pKey == NULL) {
        PRINTF_ERROR("EVP_PKEY_new err");
        ret = RSA_KEY_ERROR;
        return ret;
    }
    
    // should include <openssl/evp.h>
    // set the key referenced by pKey to rsa
    // return 1 for success or 0 for failure
    if (EVP_PKEY_set1_RSA(pKey, rsa) != 1) {
        PRINTF_ERROR("EVP_PKEY_set1_RSA err");
        ret = RSA_KEY_ERROR;
        EVP_PKEY_free(pKey);
        return ret;
    }
    
    std::string cipher_tmp;
    if (cipher_type == CIPHER_TYPE_BASE64) {
		base64_decode(cipher_data_in,cipher_tmp);
    }
    else if (cipher_type == CIPHER_TYPE_HEX) {
		std::string szCipherHex=cipher_data_in;
        hex_decode(szCipherHex, cipher_tmp);
    }
	int cipher_size = cipher_tmp.size();
    
	int clen =  RSA_size(rsa); 
	char*  clear_data =  (char* )malloc(sizeof(char)*clen);
	int clear_data_size;
    // RSA_PKCS1_OAEP_PADDING
    if (padding_mode_in == TOOLS_RSA_PKCS1_PADDING) {
        clear_data_size = RSA_public_decrypt(cipher_size, (const unsigned char* )cipher_tmp.c_str(), 
                (unsigned char* )clear_data, rsa, RSA_PKCS1_PADDING);
    }
    else if (padding_mode_in == TOOLS_RSA_PKCS1_OAEP_PADDING) {
        clear_data_size = RSA_public_decrypt(cipher_size, (const unsigned char* )cipher_tmp.c_str(), 
                (unsigned char* )clear_data, rsa, RSA_PKCS1_OAEP_PADDING);
    }
    else if (padding_mode_in == TOOLS_RSA_NO_PADDING) {
        clear_data_size = RSA_public_decrypt(cipher_size, (const unsigned char* )cipher_tmp.c_str(), 
                (unsigned char* )clear_data, rsa, RSA_NO_PADDING);
    }
    else {
    	PRINTF_ERROR("PaddingMode is invalid");
        ret = RSA_DECRYPT_ERROR;
		free(clear_data);
        EVP_PKEY_free(pKey);
        return ret;
    }
    
    if (clear_data_size < 0) {
    	PRINTF_ERROR("RSA_public_decrypt err");
        ret = RSA_DECRYPT_ERROR;
		free(clear_data);
        EVP_PKEY_free(pKey);
        return ret;
    }
    
	std::string tmp(clear_data, clear_data_size);
	clear_data_out = tmp;

	free(clear_data);
    EVP_PKEY_free(pKey);
    return 0;
}

int rsa_pubkey_decrypt(const std::string& cipher_in, 
                          int cipher_type, 
                          int padding_mode_in,
                          const std::string& pub_key_in, 
                          std::string& clear_data_out) {
    RSA* rsa = NULL;
    
    // 不成功的时候rsa没有生成, 成功以后, 后面使用rsa的要负责释放
    int ret = public_key_str2rsa(pub_key_in, rsa);
    if (0 != ret) {
        return ret;
    }

    ret = rsa_public_decrypt(rsa, cipher_in, cipher_type, padding_mode_in, clear_data_out);
    if (rsa != NULL) {
        RSA_free(rsa);
    }
    return ret;
}

int calculate_rsa_sign(const std::string& data_in, const std::string& private_key, int sign_type, std::string& signature) {
    ERR_load_crypto_strings();
    
    int ret = 0;

	RSA *rsa       =  NULL;
	BIO *bio       =  NULL;
	EVP_PKEY *evp_pkey =  NULL;
    std::string format_private_key;

	// calc digest
	unsigned char digest[SHA512_DIGEST_LENGTH];
    unsigned int digest_len = sizeof(digest);
    // should include <openssl/evp.h>
	// The EVP digest routines are a high level interface to message digests, 
	// and should be used instead of the cipher-specific functions
	EVP_MD_CTX ctx;

	unsigned int sign_size = 0;
	unsigned char*  sign = NULL;

    format_private_key = "-----BEGIN RSA PRIVATE KEY-----\n";
//	int keyLen = private_key.size();
	for(std::string::size_type i = 0; i < private_key.size(); i += 64) {
        format_private_key += private_key.substr(i, 64);
        format_private_key += '\n';
	}
	format_private_key += ("-----END RSA PRIVATE KEY-----\n");

	// 从字符串读取RSA私钥
	if ((bio = BIO_new_mem_buf((void*)(format_private_key.c_str()), format_private_key.length())) == NULL)        {
		SET_SSL_ERROR();
		ret = RSA_KEY_ERROR;
		goto ready_ret;
	}

	// 从bio结构中得到RSA结构
	rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);   
	if (NULL == rsa) {
        SET_SSL_ERROR();
		ret = RSA_KEY_ERROR;
		goto free_bio;
	}

	// should include <openssl/evp.h>
	// private key allocation, EVP_PKEY_free() frees up the private key
	evp_pkey = EVP_PKEY_new();
	if (evp_pkey == NULL) {
        SET_SSL_ERROR();
		ret = RSA_KEY_ERROR;
        goto free_rsa;
	}

	// should include <openssl/evp.h>
	// set the key referenced by pKey to rsa
	// return 1 for success or 0 for failure
	if (EVP_PKEY_set1_RSA(evp_pkey, rsa) != 1) {
        SET_SSL_ERROR();
		ret = RSA_KEY_ERROR;
		goto free_evp_pkey;
	}

	// initializes digest context ctx
	EVP_MD_CTX_init(&ctx);
	EVP_DigestInit(&ctx, EVP_sha1());
	EVP_DigestUpdate(&ctx, data_in.c_str(), data_in.size());
	EVP_DigestFinal(&ctx, digest, &digest_len);

	if (digest_len > SHA512_DIGEST_LENGTH) {
		PRINTF_ERROR("SHA256 digest length error");
        ret = RSA_PARAM_TYPE_ERROR;
        goto free_ctx;
    }

    // calc sign
	sign_size  =  RSA_size(rsa);
	sign =  (unsigned char* )malloc(sizeof(unsigned char) * sign_size);
	memset(sign, 0x0, sizeof(unsigned char) * sign_size);

	// should include <openssl/rsa.h>
	// RSA_sign() signs the message digest m of size m_len using the private key rsa as specified in PKCS #1 v2.0. 
	// It stores the signature in sigret and the signature size in siglen. sigret must point to RSA_size(rsa) bytes of memory. 
	// Note that PKCS #1 adds meta-data, placing limits on the size of the key that can be used. See RSA_private_encrypt(3) for lower-level operations.
	ret = RSA_sign(NID_sha1, digest, digest_len, sign, &sign_size, rsa);
	if (ret != 1) {
        SET_SSL_ERROR();
        ret = RSA_SIGN_ERROR;
        goto free_mem;
	}

    ret = 0;
	if (sign_type == SIGN_CODE_BASE64) {
        std::string tmp((char*)sign, sign_size);
		if (base64_encode(tmp, signature) !=0)
        {
            PRINTF_ERROR("base64 decode sign error");
            ret = RSA_PARAM_TYPE_ERROR;
            goto free_mem;
        }
	}
	else if (sign_type == SIGN_CODE_HEX) {   
        std::string tmp((char*)sign, sign_size);
        encode_hex_string(tmp, signature);
	}
	else {
		SET_SSL_ERROR();
		ret = RSA_PARAM_TYPE_ERROR;
	}

free_mem:
	free(sign);
free_ctx:
	EVP_MD_CTX_cleanup(&ctx);
free_evp_pkey:
	EVP_PKEY_free(evp_pkey);
free_rsa:
	RSA_free(rsa);
free_bio:
	BIO_free(bio);
ready_ret:
	return ret;
}


int calculate_rsa2_sign(const std::string& data_in, const std::string& private_key, int sign_type, std::string& signature) {
    ERR_load_crypto_strings();
    
    int ret = 0;

	RSA *rsa       =  NULL;
	BIO *bio       =  NULL;
	EVP_PKEY *evp_pkey =  NULL;
    std::string format_private_key;

	// calc digest
	unsigned char digest[SHA512_DIGEST_LENGTH];
    unsigned int digest_len = sizeof(digest);
    // should include <openssl/evp.h>
	// The EVP digest routines are a high level interface to message digests, 
	// and should be used instead of the cipher-specific functions
	EVP_MD_CTX ctx;

	unsigned int sign_size = 0;
	unsigned char*  sign = NULL;

    format_private_key = "-----BEGIN RSA PRIVATE KEY-----\n";
//	int keyLen = private_key.size();
	for(std::string::size_type i = 0; i < private_key.size(); i += 64) {
        format_private_key += private_key.substr(i, 64);
        format_private_key += '\n';
	}
	format_private_key += ("-----END RSA PRIVATE KEY-----\n");

	// 从字符串读取RSA私钥
	if ((bio = BIO_new_mem_buf((void*)(format_private_key.c_str()), format_private_key.length())) == NULL)        {
		SET_SSL_ERROR();
		ret = RSA_KEY_ERROR;
		goto ready_ret;
	}

	// 从bio结构中得到RSA结构
	rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);   
	if (NULL == rsa) {
        SET_SSL_ERROR();
		ret = RSA_KEY_ERROR;
		goto free_bio;
	}

	// should include <openssl/evp.h>
	// private key allocation, EVP_PKEY_free() frees up the private key
	evp_pkey = EVP_PKEY_new();
	if (evp_pkey == NULL) {
        SET_SSL_ERROR();
		ret = RSA_KEY_ERROR;
        goto free_rsa;
	}

	// should include <openssl/evp.h>
	// set the key referenced by pKey to rsa
	// return 1 for success or 0 for failure
	if (EVP_PKEY_set1_RSA(evp_pkey, rsa) != 1) {
        SET_SSL_ERROR();
		ret = RSA_KEY_ERROR;
		goto free_evp_pkey;
	}

	// initializes digest context ctx
	EVP_MD_CTX_init(&ctx);
	EVP_DigestInit(&ctx, EVP_sha256());
	EVP_DigestUpdate(&ctx, data_in.c_str(), data_in.size());
	EVP_DigestFinal(&ctx, digest, &digest_len);

	if (digest_len > SHA512_DIGEST_LENGTH) {
		PRINTF_ERROR("SHA256 digest length error");
        ret = RSA_PARAM_TYPE_ERROR;
        goto free_ctx;
    }

    // calc sign
	sign_size  =  RSA_size(rsa);
	sign =  (unsigned char* )malloc(sizeof(unsigned char) * sign_size);
	memset(sign, 0x0, sizeof(unsigned char) * sign_size);

	// should include <openssl/rsa.h>
	// RSA_sign() signs the message digest m of size m_len using the private key rsa as specified in PKCS #1 v2.0. 
	// It stores the signature in sigret and the signature size in siglen. sigret must point to RSA_size(rsa) bytes of memory. 
	// Note that PKCS #1 adds meta-data, placing limits on the size of the key that can be used. See RSA_private_encrypt(3) for lower-level operations.
	ret = RSA_sign(NID_sha256, digest, digest_len, sign, &sign_size, rsa);
	if (ret != 1) {
        SET_SSL_ERROR();
        ret = RSA_SIGN_ERROR;
        goto free_mem;
	}

    ret = 0;
	if (sign_type == SIGN_CODE_BASE64) {
        std::string tmp((char*)sign, sign_size);
		if (base64_encode(tmp, signature) !=0)
        {
            PRINTF_ERROR("base64 decode sign error");
            ret = RSA_PARAM_TYPE_ERROR;
            goto free_mem;
        }
	}
	else if (sign_type == SIGN_CODE_HEX) {   
        std::string tmp((char*)sign, sign_size);
        encode_hex_string(tmp, signature);
	}
	else {
		SET_SSL_ERROR();
		ret = RSA_PARAM_TYPE_ERROR;
	}

free_mem:
	free(sign);
free_ctx:
	EVP_MD_CTX_cleanup(&ctx);
free_evp_pkey:
	EVP_PKEY_free(evp_pkey);
free_rsa:
	RSA_free(rsa);
free_bio:
	BIO_free(bio);
ready_ret:
	return ret;
}
