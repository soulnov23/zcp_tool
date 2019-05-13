#include "printf.h"
#include "net_utils.h"
#include "utils.h"

int main(int argc, char *argv[])
{
	string ip;
	string mac;
	if (get_local_ip("eth0", ip))
	{
		PRINTF_ERROR("success");
		PRINTF_ERROR("ip:[%s]", ip.c_str());
	}
	if (get_local_mac("eth0", ip))
	{
		PRINTF_ERROR("success");
		PRINTF_ERROR("mac:[%s]", ip.c_str());
	}

	record_t record;
	record["1"] = "哈哈1";
	record["2"] = "呵呵2";
	record["3"] = "随便3";
	string buf;
	map2str(buf, record);
	PRINTF_DEBUG("buf[%s]", buf.c_str());

	record_t new_record;
	str2map(new_record, buf);
	PRINTF_DEBUG("%s %s %s", new_record["1"].c_str(), new_record["2"].c_str(), new_record["3"].c_str());
	return 0;
}
