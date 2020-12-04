#ifndef __CODER_H__
#define __CODER_H__

#include <string>

/*!
 * \fn	int upay_escape_string(const std::string& str_in, std::string& str_out);
 * \brief	Upay escape string.(escaped_string)
 * \param 		  	str_in 	The in.
 * \param [out]	str_out	The out.
 * \return	0表示成功,其余失败
 */
int upay_escape_string(const std::string& str_in, std::string& str_out);

/*!
 * \fn	int encode_hex_string(const std::string& str_in, std::string& str_out);
 * \brief	Encode hexadecimal string.
 * \param 		  	str_in 	The in.
 * \param [out]	str_out	The out.
 * \return	0表示成功,其余失败
 */
int encode_hex_string(const std::string& str_in, std::string& str_out);
/*!
 * \fn	int encode_hex_string(std::string& str_in_out);
 * \brief	Encode hexadecimal string.
 * \param [in,out]	str_in_out	The in out.
 * \return	0表示成功,其余失败
 */
int encode_hex_string(std::string& str_in_out);

/*!
 * \fn	int decode_hex_string(const std::string& str_in, std::string& str_out);
 * \brief	Decode hexadecimal string.
 * \param 		  	str_in 	The in.
 * \param [out]	str_out	The out.
 * \return	0表示成功,其余失败
 */
int decode_hex_string(const std::string& str_in, std::string& str_out);
/*!
 * \fn	int decode_hex_string(std::string& str_in_out);
 * \brief	Decode hexadecimal string.
 *
 * 等同于原strdecode
 * \param [in,out]	str_in_out	The in out.
 * \return	0表示成功,其余失败
 */
int decode_hex_string(std::string& str_in_out);

/*!
 * \fn	int buff_to_hex_string(const std::string& str_in, std::string& str_out);
 * \brief	编码为16进制字符表示
 * \param 		  	str_in 	The in.
 * \param [out]	str_out	The out.
 * \return	0表示成功,其余失败
 */
int buff_to_hex_string(const std::string& str_in, std::string& str_out);

/*!
 * \fn	int decode_hex_string_and_escape(const std::string& str_in,
 * std::string& str_out);
 * \brief	Decode hexadecimal string and
 * escape.(decode_hex_string_and_escaped_string)
 * \param 		  	str_in 	The in.
 * \param [out]	str_out	The out.
 * \return	0表示成功,其余失败
 */
int decode_hex_string_and_escape(const std::string& str_in, std::string& str_out);

/*!
 * \fn	int url_encode(const std::string& str_in, std::string& str_out);
 * \brief	URL编码(参照RFC3986)
 * \param 		  	str_in 	The in.
 * \param [out]	str_out	The out.
 * \return	0表示成功,其余失败
 */
int url_encode(const std::string& str_in, std::string& str_out);

/*!
 * \fn	int url_decode(const std::string& str_in, std::string& str_out);
 * \brief	URL解码
 * \param 		  	str_in 	The in.
 * \param [out]	str_out	The out.
 * \return	0表示成功,其余失败
 */
int url_decode(const std::string& str_in, std::string& str_out);

#endif
