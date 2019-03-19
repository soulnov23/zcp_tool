#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <string>
#include "convert.h"

int str2int(const std::string &str, int &result, int base, trim_option trim_op)
{
    const char* p_str = str.c_str();
    if (*p_str == '\0')
        return -1;

    if (!(trim_op & TRIM_START) && isspace(*p_str))
        return -1;
        
    const char *int_str_end = NULL;

    errno = 0;
    long ret = strtol(p_str, (char**)&int_str_end, base);

    if (errno != 0 || ret < INT_MIN || ret > INT_MAX) {
        return -1;
    }

    if (trim_op & TRIM_END){
        while (isspace(*int_str_end)) {
            ++int_str_end;
        }
    }

    if(*int_str_end != '\0') {
        return -1;
    }

    result = (int)ret;
    return 0;
}

template <typename TI, typename TO>
void convert_to(const TI &in, TO &out)
{
	std::stringstream srm;
	srm << in;
	srm >> out;
}

/*
template <typename T>
std::string to_string(const T &value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}
*/
*/
