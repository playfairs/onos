#include "console.h"

#include <fcntl.h>
#include <unistd.h>

int onos_console_open(struct onos_console *console, const char *path)
{
	console->fd = open(path, O_RDWR | O_NOCTTY);
	return console->fd < 0 ? -1 : 0;
}

int onos_console_attach_stdio(const struct onos_console *console)
{
	if (dup2(console->fd, STDIN_FILENO) < 0 ||
		dup2(console->fd, STDOUT_FILENO) < 0 ||
		dup2(console->fd, STDERR_FILENO) < 0) {
		return -1;
	}
	return 0;
}
