#include "console.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <termios.h>
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

int onos_console_verify(const struct onos_console *console)
{
	struct stat descriptor;
	struct termios attributes;

	return isatty(console->fd) && fstat(console->fd, &descriptor) == 0 &&
		tcgetattr(console->fd, &attributes) == 0 ? 0 : -1;
}

int onos_console_verify_stdio(void)
{
	struct onos_console descriptor;

	for (int fd = STDIN_FILENO; fd <= STDERR_FILENO; fd++) {
		descriptor.fd = fd;
		if (onos_console_verify(&descriptor) != 0) {
			return -1;
		}
	}
	return 0;
}
