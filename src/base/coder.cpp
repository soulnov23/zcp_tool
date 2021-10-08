#include "src/base/coder.h"

#include <string.h>

// typedef std::string::size_type (*escape_function)(const std::string& str_in,
//                                                  std::string::size_type pos,
//                                                  std::string& str_out);
using escape_function = std::string::size_type (*)(const std::string&, std::string::size_type, std::string&);

// typedef std::string::size_type (*escape_predicator)(
//    const std::string& str, std::string::size_type start_pos);
using escape_predicator = std::string::size_type (*)(const std::string&, std::string::size_type);

template <escape_predicator NEXT_ESCAPE_POS, escape_function ESCAPE_FUN>
int string_escape(const std::string& str_in, std::string& str_out) {
    std::string::size_type sp = 0, cur, used;
    std::string esc_str;
    while ((cur = NEXT_ESCAPE_POS(str_in, sp)) != std::string::npos) {
        esc_str.append(str_in, sp, cur - sp);
        used = ESCAPE_FUN(str_in, cur, esc_str);
        if (used == std::string::npos) {
            return -1;
        }
        sp = cur + used;
    }
    esc_str.append(str_in, sp, str_in.length() - sp);
    str_out = esc_str;
    return 0;
}

template <escape_function ESCAPE_FUN>
int string_escape(const std::string& str_in, std::string& str_out) {
    std::string::size_type sp = 0, used;
    std::string esc_str;
    while (sp < str_in.length()) {
        used = ESCAPE_FUN(str_in, sp, esc_str);
        if (used == std::string::npos) {
            return -1;
        }
        sp += used;
    }
    str_out = esc_str;
    return 0;
}

template <const char* SPECIAL_CHARS>
std::string::size_type escape_with_char_set(const std::string& str_in, std::string::size_type start_pos) {
    const char* pstr = strpbrk(str_in.c_str() + start_pos, SPECIAL_CHARS);
    if (pstr == nullptr) return std::string::npos;
    return pstr - str_in.c_str();
}

template <const bool* SHOULD_ESCAPE>
std::string::size_type escape_with_char_set(const std::string& str_in, std::string::size_type start_pos) {
    for (; start_pos < str_in.length(); ++start_pos) {
        if (SHOULD_ESCAPE[(unsigned char)str_in[start_pos]]) return start_pos;
    }
    return std::string::npos;
}

template <std::string::size_type REMAIN_CNT>
void char_append(const char* chars, std::string& str) {
    str.push_back(*chars);
    char_append<REMAIN_CNT - 1>(chars + 1, str);
}

template <>
void char_append<0>(const char* chars, std::string& str) {
    (void)chars;
    (void)str;
}

// typedef bool (*fixed_size_escape_function)(const std::string& str_in,
//                                           std::string::size_type pos,
//                                           char* chars_out);
using fixed_size_escape_function = bool (*)(const std::string&, std::string::size_type, char*);

template <fixed_size_escape_function FIXED_SIZE_ESCAPE_FUN, std::string::size_type OUT_FIXED_SIZE,
          std::string::size_type IN_FIXED_SIZE>
std::string::size_type fixed_size_escape(const std::string& str_in, std::string::size_type pos, std::string& str_out) {
    char chars_out[OUT_FIXED_SIZE];
    if (pos + IN_FIXED_SIZE > str_in.length() || !FIXED_SIZE_ESCAPE_FUN(str_in, pos, chars_out)) return std::string::npos;
    char_append<OUT_FIXED_SIZE>(chars_out, str_out);
    return IN_FIXED_SIZE;
}

template <fixed_size_escape_function FIXED_SIZE_ESCAPE_FUN, std::string::size_type OUT_FIXED_SIZE>
std::string::size_type fixed_size_escape(const std::string& str_in, std::string::size_type pos, std::string& str_out) {
    char chars_out[OUT_FIXED_SIZE];
    FIXED_SIZE_ESCAPE_FUN(str_in, pos, chars_out);
    char_append<OUT_FIXED_SIZE>(chars_out, str_out);
    return 1;
}

// end

// upay_escape_string

bool upay_escape_char(const std::string& str_in, std::string::size_type pos, char* t) {
    const char& c = str_in[pos];
    if (c == '\\')
        *t = '_';
    else if (c == '|')
        *t = '&';
    else
        *t = ' ';
    return true;
}

extern const char upay_escape_chars[] = "\\|\r\n";

// end

// encode_hex_string

extern const bool hex_string_encode_table[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

bool hex_string_encode_char(const std::string& str_in, std::string::size_type pos, char* t) {
    static const char* hex_table = "0123456789ABCDEF";
    const unsigned char& c = str_in[pos];
    t[0] = '%';
    t[1] = hex_table[(c >> 4) & 0x0F];
    t[2] = hex_table[(c & 0x0F)];
    return true;
}

// end

// buff_to_hex_string

bool buff_hex_string_encode_char(const std::string& str_in, std::string::size_type pos, char* t) {
    static const char* hex_table = "0123456789ABCDEF";
    const unsigned char& c = str_in[pos];
    t[0] = hex_table[(c >> 4) & 0x0F];
    t[1] = hex_table[(c & 0x0F)];
    return true;
}

// end

// decode_hex_string

bool hex_string_part_decode(const std::string& str_in, std::string::size_type pos, char* t) {
    static int hex_index_table[] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
        -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

    int hi = hex_index_table[(unsigned char)str_in[pos + 1]];
    int lo = hex_index_table[(unsigned char)str_in[pos + 2]];

    if (~hi && ~lo) {
        *t = (hi << 4) | lo;
        return true;
    }
    return false;
}

extern const char hex_string_prefix[] = "%";

// end

// url_encode

extern const bool url_encode_table[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

int upay_escape_string(const std::string& str_in, std::string& str_out) {
    return string_escape<escape_with_char_set<upay_escape_chars>, fixed_size_escape<upay_escape_char, 1>>(str_in, str_out);
}

int encode_hex_string(const std::string& str_in, std::string& str_out) {
    return string_escape<escape_with_char_set<hex_string_encode_table>, fixed_size_escape<hex_string_encode_char, 3>>(str_in,
                                                                                                                      str_out);
}

int encode_hex_string(std::string& str_in_out) { return encode_hex_string(str_in_out, str_in_out); }

int decode_hex_string(const std::string& str_in, std::string& str_out) {
    return string_escape<escape_with_char_set<hex_string_prefix>, fixed_size_escape<hex_string_part_decode, 1, 3>>(str_in,
                                                                                                                   str_out);
}

int decode_hex_string(std::string& str_in_out) { return decode_hex_string(str_in_out, str_in_out); }

int buff_to_hex_string(const std::string& str_in, std::string& str_out) {
    return string_escape<fixed_size_escape<buff_hex_string_encode_char, 2>>(str_in, str_out);
}

int decode_hex_string_and_escape(const std::string& str_in, std::string& str_out) {
    std::string str_mid;
    int ret = decode_hex_string(str_in, str_mid);
    return ret == 0 ? upay_escape_string(str_mid, str_out) : ret;
}

int url_encode(const std::string& str_in, std::string& str_out) {
    return string_escape<escape_with_char_set<url_encode_table>, fixed_size_escape<hex_string_encode_char, 3>>(str_in, str_out);
}

int url_decode(const std::string& str_in, std::string& str_out) { return decode_hex_string(str_in, str_out); }
