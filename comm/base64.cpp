#include "base64.h"

int base64_encode(const string &str_in, string &str_out)
{
    static const char base64_encode_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    string res;
    res.reserve((str_in.length() + 2) / 3 * 4 + 1); 
    string::size_type i = 0;
    for (; i + 3 <= str_in.length(); i += 3) 
	{
        res.push_back(base64_encode_table[((unsigned char)str_in[i]) >> 2]);
        res.push_back(base64_encode_table[(((((unsigned char)str_in[i]) & 0x03) << 4) | (((unsigned char)str_in[i + 1]) >> 4))]);
        res.push_back(base64_encode_table[((((unsigned char)str_in[i + 1]) & 0x0F) << 2) | (((unsigned char)str_in[i + 2]) >> 6)]);
        res.push_back(base64_encode_table[((unsigned char)str_in[i + 2]) & 0x3F]);
    }
    if (i + 1 == str_in.length()) 
	{
        res.push_back(base64_encode_table[((unsigned char)str_in[i]) >> 2]);
        res.push_back(base64_encode_table[((((unsigned char)str_in[i]) & 0x03) << 4)]);
        res.push_back('=');
        res.push_back('=');
    }
    else if (i + 2 == str_in.length()) 
	{
        res.push_back(base64_encode_table[((unsigned char)str_in[i]) >> 2]);
        res.push_back(base64_encode_table[(((((unsigned char)str_in[i]) & 0x03) << 4) | (((unsigned char)str_in[i + 1]) >> 4))]);
        res.push_back(base64_encode_table[((((unsigned char)str_in[i + 1]) & 0x0F) << 2)]);
        res.push_back('=');
    }
    str_out = res;
    return 0;
}

int base64_decode(const string &str_in, string &str_out)
{
    static const int base64_decode_table[] = 
	{
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
        52,53,54,55,56,57,58,59,60,61,-1,-1,-1,0 ,-1,-1,
        -1,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10,11,12,13,14,
        15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
        -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    };

    if ((str_in.length() & 0x3) != 0) 
	{
        return -1;
    }

    string::size_type last_eq = str_in.find('=');
    if (last_eq != string::npos) 
	{
        if (last_eq + 2 < str_in.length()) 
		{
            return -1;
        }
        else if (last_eq + 2 == str_in.length() && str_in[last_eq + 1] != '=')
		{
            return -1;
        }
    }

    string res;
    res.reserve(str_in.length() / 4 * 3 + 1); 
    string::size_type pos = 0;
    int i, j, k, l;
    for (;pos + 4 < str_in.length(); pos += 4) 
	{
        i = base64_decode_table[(unsigned char)str_in[pos]];
        j = base64_decode_table[(unsigned char)str_in[pos + 1]];
        k = base64_decode_table[(unsigned char)str_in[pos + 2]];
        l = base64_decode_table[(unsigned char)str_in[pos + 3]];

        if (!(~i && ~j && ~k && ~l)) 
		{
            return -1;
        }
        res.push_back((i << 2) | (j >> 4));
        res.push_back(((j & 0x1F) << 4) | (k >> 2));
        res.push_back(((k & 0x3) << 6) | l);
    }

    i = base64_decode_table[(unsigned char)str_in[pos]];
    j = base64_decode_table[(unsigned char)str_in[pos + 1]];
    k = base64_decode_table[(unsigned char)str_in[pos + 2]];
    l = base64_decode_table[(unsigned char)str_in[pos + 3]];

    if (str_in[pos + 2] == '=') 
	{
        if (j & 0xF) 
		{
            return -1;
        }
        res.push_back((i << 2) | (j >> 4));
    }
    else if (str_in[pos + 3] == '=') 
	{
        if ((k & 0x3)) 
		{
            return -1;
        }
        res.push_back((i << 2) | (j >> 4));
        res.push_back(((j & 0x1F) << 4) | (k >> 2));
    }
    else 
	{
        res.push_back((i << 2) | (j >> 4));
        res.push_back(((j & 0x1F) << 4) | (k >> 2));
        res.push_back(((k & 0x3) << 6) | l);
    }
    str_out = res;
    return 0;
}  