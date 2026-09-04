#include "session.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

int onos_session_init(struct onos_session *session, int console_fd)
{
	if (!isatty(console_fd)) {
		return -1;
	}
	session->console_fd = console_fd;
	session->shell_pid = -1;
	return 0;
}

int onos_session_verify(const struct onos_session *session)
{
	if (tcgetpgrp(session->console_fd) != getpgrp()) {
		errno = EPERM;
		return -1;
	}
	return 0;
}

int onos_session_spawn_shell(struct onos_session *session, const char *path)
{
	int startup[2];
	unsigned char error_number;
	ssize_t received;

	if (pipe(startup) != 0) {
		return -1;
	}
	if (fcntl(startup[1], F_SETFD, FD_CLOEXEC) != 0) {
		close(startup[0]);
		close(startup[1]);
		return -1;
	}
	session->shell_pid = fork();
	if (session->shell_pid < 0) {
		close(startup[0]);
		close(startup[1]);
		return -1;
	}
	if (session->shell_pid != 0) {
		close(startup[1]);
		received = read(startup[0], &error_number, sizeof(error_number));
		close(startup[0]);
		if (received < 0 || received == sizeof(error_number)) {
			errno = received < 0 ? errno : error_number;
			return -1;
		}
		return received == 0 ? 0 : (errno = EIO, -1);
	}
	close(startup[0]);

	if (setsid() < 0 || ioctl(session->console_fd, TIOCSCTTY, 0) < 0 ||
		dup2(session->console_fd, STDIN_FILENO) < 0 ||
		dup2(session->console_fd, STDOUT_FILENO) < 0 ||
		dup2(session->console_fd, STDERR_FILENO) < 0 ||
		onos_session_verify(session) != 0) {
		error_number = (unsigned char)errno;
		(void)write(startup[1], &error_number, sizeof(error_number));
		_exit(126);
	}

	if (execl(path, path, (char *)NULL) < 0) {
		error_number = (unsigned char)errno;
		(void)write(startup[1], &error_number, sizeof(error_number));
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
