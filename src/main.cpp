#include "server.h"
#include "client.h"
#include <string.h>

int main(int argc, char *argv[])
{
	char *pos = strrchr(argv[0], '/');
	if (pos)
	{
		pos++;
	}
	if (strcmp(pos, "server") == 0)
	{
		server::get_instance()->start();
	}
	else if (strcmp(pos, "client") == 0)
	{
		client::get_instance()->start();
	}
	return 0;
}
