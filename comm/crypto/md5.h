#ifndef __MD5_H__
#define __MD5_H__

#include <string>

/*!
 * \enum	md5_option
 * \brief	md5摘要算法选项
*/
enum md5_option {
    MD5_RAW = 0,  //!< 直接返回原始结果
    MD5_HEX = 1   //!< 结果经过Hex编码后返回
};

/*!
 * \fn	std::string md5(const std::string& data_in, int mode = MD5_HEX);
 * \brief	MD5信息摘要算法
 * \param	data_in 原始数据
 * \param	mode   	(Optional) 选项,见\a upay::crypto::md5_option
 * \return	经MD5摘要后的结果(可能经过编码,见\a mode 参数)
*/
std::string md5(const std::string& data_in, int mode = MD5_HEX);

#endif
