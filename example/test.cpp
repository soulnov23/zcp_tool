#include "printf.h"
#include <thread>
using namespace std;

class test
{
public:
	static int print(const char *str1, const char *str2)
	{
		PRINTF_DEBUG("test::print %s %s", str1, str2);
		return 0;
	}
};

int print(int n1, int n2)
{
	PRINTF_ERROR("print %d %d", n1, n2);
	return 0;
}
int main(int argc, char *argv[])
{
	thread t1(print, 1, 2);
	thread t2(test::print, "hello", "world");
	t1.join();
	t2.join();
	return 0;
}
