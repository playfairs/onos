#ifndef ONOS_TTY_H
#define ONOS_TTY_H

#include <sys/types.h>
#include <termios.h>

struct onos_tty {
	int fd;
	struct termios saved;
	int saved_valid;
};

int onos_tty_init(struct onos_tty *tty, int fd);
int onos_tty_set_foreground(const struct onos_tty *tty, pid_t process_group);
ssize_t onos_tty_read(struct onos_tty *tty, void *buffer, size_t size);
ssize_t onos_tty_write(const struct onos_tty *tty, const void *buffer, size_t size);
void onos_tty_restore(struct onos_tty *tty);

#endif
