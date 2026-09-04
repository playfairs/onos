#ifndef ONOS_SESSION_H
#define ONOS_SESSION_H

#include <sys/types.h>

struct onos_session {
	int console_fd;
	pid_t shell_pid;
};

int onos_session_init(struct onos_session *session, int console_fd);
int onos_session_verify(const struct onos_session *session);
int onos_session_spawn_shell(struct onos_session *session, const char *path);
int onos_session_wait(struct onos_session *session, int *status);

#endif
