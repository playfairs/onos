#include <stdio.h>

#include "console.h"
#include "mounts.h"
#include "rootfs.h"
#include "session.h"

#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
	struct onos_console console;
	struct onos_session session;
	int status;
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	puts("[ONOS] init starting");
	if (onos_mount_virtual_filesystems() != 0) {
		fputs("[ONOS] failed to mount virtual filesystems\n", stderr);
		return EXIT_FAILURE;
	}

	puts("[ONOS] initializing /dev");
	if (onos_prepare_root_filesystem() != 0) {
		perror("[ONOS] /dev");
		return EXIT_FAILURE;
	}

	puts("[ONOS] initializing console");
	if (onos_console_open(&console, "/dev/console") != 0 ||
		onos_console_attach_stdio(&console) != 0) {
		perror("[ONOS] console");
		return EXIT_FAILURE;
	}

	puts("[ONOS] initializing TTY");
	if (onos_session_init(&session, console.fd) != 0) {
		perror("[ONOS] session");
		return EXIT_FAILURE;
	}

	for (;;) {
		puts("[ONOS] starting shell");
		if (onos_session_spawn_shell(&session, "/bin/onos-shell") != 0) {
			perror("[ONOS] shell");
			return EXIT_FAILURE;
		}

		if (onos_session_wait(&session, &status) != 0) {
			perror("[ONOS] waitpid");
			return EXIT_FAILURE;
		}

		if (WIFEXITED(status)) {
			printf("[ONOS] shell exited with status %d; restarting\n",
				WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
			printf("[ONOS] shell terminated by signal %d; restarting\n",
				WTERMSIG(status));
		}
	}
}
