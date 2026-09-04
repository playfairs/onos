#include "session.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>

int onos_session_init(struct onos_session *session, int console_fd)
{
	session->console_fd = console_fd;
	session->shell_pid = -1;
	return 0;
}

int onos_session_spawn_shell(struct onos_session *session, const char *path)
{
	session->shell_pid = fork();
	if (session->shell_pid < 0) {
		return -1;
	}
	if (session->shell_pid != 0) {
		return 0;
	}

	if (setsid() < 0 || ioctl(session->console_fd, TIOCSCTTY, 0) < 0 ||
		dup2(session->console_fd, STDIN_FILENO) < 0 ||
		dup2(session->console_fd, STDOUT_FILENO) < 0 ||
		dup2(session->console_fd, STDERR_FILENO) < 0) {
		_exit(126);
	}

	if (execl(path, path, (char *)NULL) < 0) {
		dprintf(STDERR_FILENO, "[ONOS] exec %s failed: %s\n", path,
			strerror(errno));
		_exit(127);
	}
	return 0;
}

int onos_session_wait(struct onos_session *session, int *status)
{
	pid_t waited;
	do {
		waited = waitpid(session->shell_pid, status, 0);
	} while (waited < 0 && errno == EINTR);

	if (waited < 0) {
		return -1;
	}

	while (waitpid(-1, NULL, WNOHANG) > 0) {
	}
	return 0;
}
