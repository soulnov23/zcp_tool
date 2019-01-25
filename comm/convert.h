#ifndef __CONVERT_H__
#define __CONVERT_H__

#include <string>

/*!
 * \enum	trim_option
 * \brief	对字符串空白的修正选项
*/
enum trim_option
{
    TRIM_NONE = 0x0,	//!< 保留所有空白
    TRIM_START = 0x1,   //!< 去除头部空白
    TRIM_END = 0x2, //!< 去除尾部空白
    TRIM_START_END = TRIM_START | TRIM_END  //!< 取出头尾空白,等同于 \a upay::util::TRIM_START | \a upay::util::TRIM_END
};
/*!
 * \fn	int str2int(const std::string &str, int &result, int base = 10, trim_option trim_op = TRIM_NONE);
 * \brief	将字符串转换为整型(int)
 * \param 	   	str	   	待转换字符串.
 * \param [out]	result 	整数结果.
 * \param 	   	base   	(Optional) 进制,默认为10.
 * \param 	   	trim_op	(Optional) 字符串空白修正选项,见\a upay::util::trim_option.
 * \return	\a 0为成功,否则失败
*/
int str2int(const std::string &str, int &result, int base = 10, trim_option trim_op = TRIM_NONE);

/*!
 * \fn	template <typename TI, typename TO> inline void convert_to(const TI& in, TO &out)
 * \brief	数据间转换
 *
 * 该函数无法报告错误
 * \tparam	TI	源数据类型
 * \tparam	TO	目标数据类型
 * \param 		  	in 	源数据
 * \param [out]	out	转换结果
*/
template <typename TI, typename TO>
void convert_to(const TI &in, TO &out);

/*!
 * \fn	template <typename T> inline std::string to_string(const T& value)
 * \brief	转换为字符串
 *
 * 该函数无法报告错误
 * \tparam	T	源数据类型
 * \param	value	源数据
 * \return	转换后的字符串
*/
/*
template <typename T>
std::string to_string(const T &value);
*/

#endif
