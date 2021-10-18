#include "src/base/string_utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <sstream>

#include "src/base/coder.h"

#define FIELD_FLAG "&"
#define VALUE_FLAG "="

#define HEADER_FLAG  "\r\n"
#define HEADER_VALUE ": "

/*
static string& ltrim(string& ss, int (*pf)(int)=isspace) {
    string::iterator p = find_if(ss.begin(), ss.end(), not1(ptr_fun(pf)));
    ss.erase(ss.begin(), p);
    return ss;
}

static string& rtrim(string& ss, int (*pf)(int)=isspace) {
    string::reverse_iterator p = find_if(ss.rbegin(), ss.rend(),
not1(ptr_fun(pf)));
    ss.erase(p.base(), ss.end());
    return ss;
}

void string_trim(string& str) {
        ltrim(rtrim(str));
}

std::string int_to_string(int i) {
        char szTemp[64];
        snprintf(szTemp,sizeof(szTemp),"%d",i);
        return std::string(szTemp);
}

//%m.nf m表示输出数据总宽度过大左边补空格（包括小数点.）n表示数据精度
//%.nf 总长度不限小数点后面保留n位不足补零
std::string double_to_string(double i) {
        char szTemp[64];
        snprintf(szTemp,sizeof(szTemp),"%.2f",i);
        return std::string(szTemp);
}

std::string uint_to_string(unsigned int i) {
        char szTemp[64];
        snprintf(szTemp,sizeof(szTemp),"%u",i);
        return std::string(szTemp);
}
*/

void string_replace(string& data, const string& src, const string& dst) {
    string::size_type pos = 0;
    string::size_type srclen = src.size();
    string::size_type dstlen = dst.size();
    while ((pos = data.find(src, pos)) != string::npos) {
        data.replace(pos, srclen, dst);
        pos += dstlen;
    }
}

void string_lower(string& str) {
    for (auto& i : str) {
        i = tolower(i);
    }
}

void string_upper(string& str) {
    for (auto& i : str) {
        i = toupper(i);
    }
}

void str2vec(const string& buf, const char& field, vector_t& vec) {
    vec.clear();
    stringstream input(buf);
    string value;
    while (getline(input, value, field)) {
        vec.push_back(value);
    }
}

void str2vec(const string& buf, const string& field, vector_t& vec) {
    vec.clear();
    size_t offset = 0;
    size_t next = 0;
    while (true) {
        next = buf.find_first_of(field, offset);
        if (next == string::npos) {
            vec.push_back(buf.substr(offset));
            break;
        }
        vec.push_back(buf.substr(offset, next - offset));
        offset = next + 1;
    }
}

void map2str(string& buf, const record_t& record, bool encode /*=true*/) {
    for (record_t::const_iterator it = record.begin(); it != record.end(); it++) {
        buf += FIELD_FLAG + it->first;
        if (encode) {
            string result;
            url_encode(it->second, result);
            buf += VALUE_FLAG + result;
        } else {
            buf += VALUE_FLAG + it->second;
        }
    }

    if (!buf.empty()) {
        buf = buf.substr(1);
    }
}

static bool get_value(const string& kv, string& key, string& value, const string& field, bool decode = true) {
    size_t pos = kv.find(field);
    if (pos == string::npos) {
        return false;
    }
    key = kv.substr(0, pos);
    value = kv.substr(pos + field.length());
    if (decode) {
        string result;
        url_decode(value, result);
        value = result;
    }
    return true;
}

static bool get_value(const string& kv, string& key, string& value, const char& field, bool decode = true) {
    stringstream input(kv);
    if (getline(input, key, field)) {
        if (getline(input, value, field)) {
            if (decode) {
                string result;
                url_decode(value, result);
                value = result;
            }
            return true;
        }
    }
    return false;
}

void str2map(record_t& record, const string& buf, bool decode /*=true*/) {
    size_t offset = 0;
    size_t next = 0;
    string kv;
    string key;
    string value;
    while (true) {
        next = buf.find_first_of(FIELD_FLAG, offset);
        if (next == string::npos) {
            kv = buf.substr(offset);
            if (get_value(kv, key, value, VALUE_FLAG, decode)) {
                record[key] = value;
            }
            break;
        }
        kv = buf.substr(offset, next - offset);
        if (get_value(kv, key, value, VALUE_FLAG, decode)) {
            record[key] = value;
        }
        offset = next + 1;
    }
}

bool va_function(const char* format, ...) {
    char* message = nullptr;
    va_list args;
    va_start(args, format);
    if (vasprintf(&message, format, args) == -1) {
        va_end(args);
        return false;
    }
    va_end(args);
    /* message has been allocated by vasprintf above, and needs free */
    free(message);
    return true;
}

void str_format(string& dst, const char* format, ...) {
    dst = "";
    if (format == nullptr) {
        return;
    }
    va_list args;
    va_start(args, format);
    size_t len = vsnprintf(nullptr, 0, format, args);
    if (len > 0) {
        va_list args_tmp;
        va_start(args_tmp, format);

        dst.resize(len);
        char* tmp = (char*)dst.c_str();
        vsnprintf(tmp, len + 1, format, args_tmp);

        va_end(args_tmp);
    }
    va_end(args);
}

void escape_string(const string& sql, string& dest) {
    char escape;
    for (auto character : sql) {
        switch (character) {
            case 0: /* Must be escaped for 'mysql' */
                escape = '0';
                break;
            case '\n': /* Must be escaped for logs */
                escape = 'n';
                break;
            case '\r':
                escape = 'r';
                break;
            case '\\':
                escape = '\\';
                break;
            case '\'':
                escape = '\'';
                break;
            case '"': /* Better safe than sorry */
                escape = '"';
                break;
            case '\032': /* This gives problems on Win32 */
                escape = 'Z';
                break;
            default:
                escape = 0;
        }
        if (escape != 0) {
            dest += '\\';
            dest += escape;
        } else {
            dest += character;
        }
    }
}

unsigned char hex2byte(char c) {
    if (c >= '0' && c <= '9') {
        return (unsigned char)(c - '0');
    } else if (c >= 'a' && c <= 'f') {
        return (unsigned char)(10 + c - 'a');
    } else if (c >= 'A' && c <= 'F') {
        return (unsigned char)(10 + c - 'A');
    } else {  //出现异常字符
        return 0xff;
    }
}

void hex2str(string& dst, const string& src) {
    dst = "";
    unsigned char high, low;
    const char* s = src.c_str();
    while (*s) {
        //高4位
        high = hex2byte(*s++);
        if (high == 0xff || *s == '\0') {
            return;
        }
        //低4位
        low = hex2byte(*s++);
        if (low == 0xff) {
            return;
        }
        dst.push_back((char)((high << 4) | low));
    }
}

void str2hex(string& dst, const string& src) {
    dst = "";
    stringstream ss;
    char hex[3] = {0};
    for (auto s : src) {
        snprintf(hex, sizeof(hex), "%02x", (unsigned char)s);
        ss << hex;
    }
    dst = ss.str();
}