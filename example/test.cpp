#include "printf_utils.h"
#include <string>
#include <sstream>
using namespace std;

template <typename T>
string toString(const T& value)
{
    ostringstream oss;
    oss << value;
    return oss.str();
}

int main(int argc, char* argv[])
{
	//string amount = "1194900";
	string amount = "11111";
	//转换成元
	char temp_amount[64] = {'\0'};
    snprintf(temp_amount, sizeof(temp_amount), "%.2lf", atof(amount.c_str())/100.00);
    PRINTF_DEBUG("amount[%s]", temp_amount);
	amount = temp_amount;
	string new_amout = toString(atof(amount.c_str())*1000);
	PRINTF_DEBUG("amount[%s]", new_amout.c_str());

	string new_amout2 = to_string(atof(amount.c_str())*1000);
	PRINTF_DEBUG("amount[%s]", new_amout2.c_str());

	char temp_amount2[64] = {'\0'};
    snprintf(temp_amount2, sizeof(temp_amount2), "%.0lf", atof(amount.c_str())*1000);
	PRINTF_DEBUG("amount[%s]", temp_amount2);
}