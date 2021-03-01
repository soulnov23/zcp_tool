#include "zcp_tool/comm/utf8_utils.h"

#include <alloca.h>
#include <iconv.h>

#include "zcp_tool/comm/printf_utils.h"

static const char trailingBytesForUTF8[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5};

bool is_legal_utf8(const string& data) {
    const unsigned char* data_begin = (const unsigned char*)data.c_str();
    const unsigned char* data_end   = data_begin + data.length();
    int length                      = trailingBytesForUTF8[*data_begin] + 1;
    if (data_begin + length > data_end) {
        return false;
    }
    unsigned char a;
    const unsigned char* srcptr = data_begin + length;
    switch (length) {
        default:
            return false;
        /* Everything else falls through when "true"... */
        case 4:
            if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;  // @suppress("No break at end of case")
        case 3:
            if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;  // @suppress("No break at end of case")
        case 2:
            if ((a = (*--srcptr)) > 0xBF) return false;

            switch (*data_begin) {
                /* no fall-through in this inner switch */
                case 0xE0:
                    if (a < 0xA0) return false;
                    break;
                case 0xED:
                    if (a > 0x9F) return false;
                    break;
                case 0xF0:
                    if (a < 0x90) return false;
                    break;
                case 0xF4:
                    if (a > 0x8F) return false;
                    break;
                default:
                    if (a < 0x80) return false;
            }  // @suppress("No break at end of case")

        case 1:
            if (*data_begin >= 0x80 && *data_begin < 0xC2) return false;
    }
    if (*data_begin > 0xF4) return false;
    return true;
}

string to_utf8(const char* charset, const string& data) {
    iconv_t conv = iconv_open("UTF-8", charset);
    if (conv == (iconv_t)-1) {
        PRINTF_ERROR("iconv_open error");
        return "";
    }
    const char* str = data.c_str();
    size_t len      = data.size();
    size_t max_len  = len * 2;
    size_t out_len  = max_len;
    char* out_str   = (char*)alloca(max_len + 16);
    if (out_str == nullptr) {
        iconv_close(conv);
        PRINTF_ERROR("alloca error");
        return "";
    }
    int ret = iconv(conv, (char**)&str, &len, &out_str, &out_len);
    if (ret == -1) {
        iconv_close(conv);
        PRINTF_ERROR("iconv error");
        return "";
    }
    if (out_len > max_len) {
        iconv_close(conv);
        PRINTF_ERROR("buffer lenth not enough");
        return "";
    }
    iconv_close(conv);
    return string(out_str, out_len);
}

string utf8_to(const char* charset, const string& data) {
    iconv_t conv = iconv_open(charset, "UTF-8");
    if (conv == (iconv_t)-1) {
        PRINTF_ERROR("iconv_open error");
        return "";
    }
    const char* str = data.c_str();
    size_t len      = data.size();
    size_t max_len  = len * 4;
    size_t out_len  = max_len;
    char* out_str   = (char*)alloca(max_len + 16);
    if (out_str == nullptr) {
        iconv_close(conv);
        PRINTF_ERROR("alloca error");
        return "";
    }
    int ret = iconv(conv, (char**)&str, &len, &out_str, &out_len);
    if (ret == -1) {
        iconv_close(conv);
        PRINTF_ERROR("iconv error");
        return "";
    }
    if (out_len > max_len) {
        iconv_close(conv);
        PRINTF_ERROR("buffer lenth not enough");
        return "";
    }
    iconv_close(conv);
    return string(out_str, out_len);
}