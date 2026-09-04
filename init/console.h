#ifndef ONOS_CONSOLE_H
#define ONOS_CONSOLE_H

struct onos_console {
	int fd;
};

int onos_console_open(struct onos_console *console, const char *path);
int onos_console_attach_stdio(const struct onos_console *console);

#endif
