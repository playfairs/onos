#include "tty.h"

#include <errno.h>
#include <unistd.h>

int onos_tty_init(struct onos_tty *tty, int fd)
{
	struct termios current;

	tty->fd = fd;
	tty->saved_valid = tcgetattr(fd, &tty->saved) == 0;
	if (!tty->saved_valid || tcgetattr(fd, &current) < 0) {
		return -1;
	}

	current.c_lflag |= ICANON | ECHO | ECHOE | ISIG | IEXTEN;
	current.c_iflag |= ICRNL;
	current.c_oflag |= OPOST | ONLCR;
	current.c_cc[VERASE] = 0x7f;
	current.c_cc[VEOF] = 0x04;
	current.c_cc[VINTR] = 0x03;
	current.c_cc[VMIN] = 1;
	current.c_cc[VTIME] = 0;

	if (tcsetattr(fd, TCSANOW, &current) != 0) {
		return -1;
	}
	return onos_tty_verify(tty);
}

int onos_tty_verify(const struct onos_tty *tty)
{
	struct termios current;

	if (!isatty(tty->fd) || tcgetattr(tty->fd, &current) != 0) {
		return -1;
	}
	if ((current.c_lflag & (ICANON | ECHO | ISIG)) !=
		(ICANON | ECHO | ISIG) || !(current.c_iflag & ICRNL) ||
		!(current.c_oflag & (OPOST | ONLCR)) || current.c_cc[VEOF] != 0x04 ||
		current.c_cc[VINTR] != 0x03 || current.c_cc[VMIN] != 1 ||
		current.c_cc[VTIME] != 0) {
		errno = EINVAL;
		return -1;
	}
	return 0;
}

int onos_tty_set_foreground(const struct onos_tty *tty, pid_t process_group)
{
	return tcsetpgrp(tty->fd, process_group);
}

ssize_t onos_tty_read(struct onos_tty *tty, void *buffer, size_t size)
{
	ssize_t result;
	do {
		result = read(tty->fd, buffer, size);
	} while (result < 0 && errno == EINTR);
	return result;
}

ssize_t onos_tty_write(const struct onos_tty *tty, const void *buffer, size_t size)
{
	const char *bytes = buffer;
	size_t written = 0;

	while (written < size) {
		ssize_t result = write(tty->fd, bytes + written, size - written);
		if (result < 0) {
			if (errno == EINTR) {
				continue;
			}
			return -1;
		}
		written += (size_t)result;
	}
	return (ssize_t)written;
}

void onos_tty_restore(struct onos_tty *tty)
{
	if (tty->saved_valid) {
		tcsetattr(tty->fd, TCSANOW, &tty->saved);
	}
}
