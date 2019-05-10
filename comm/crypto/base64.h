#ifndef __BASE64_H__
#define __BASE64_H__

#include <string>

/*!
 * \fn	int base64_encode(const std::string &str_in, std::string &str_out);
 * \brief	Base64编码
 * \param 		  	str_in 	The in.
 * \param [out]	str_out	The out.
 * \return	0表示成功,其余失败
*/
int base64_encode(const std::string &str_in, std::string &str_out);

/*!
 * \fn	int base64_decode(const std::string &str_in, std::string &str_out);
 * \brief	Base64解码
 *
 * 带严格检查的Base64解码
 * \param 		  	str_in 	The in.
 * \param [out]	str_out	The out.
 * \return	0表示成功,其余失败
*/
int base64_decode(const std::string &str_in, std::string &str_out);

#endif
