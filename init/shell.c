#include "tty.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define ONOS_LINE_SIZE 4096
#define ONOS_ARGC_MAX 64

static struct onos_tty shell_tty;

static void onos_shell_print_prompt(void)
{
	char directory[ONOS_LINE_SIZE];
	if (getcwd(directory, sizeof(directory)) == NULL) {
		strcpy(directory, "?");
	}
	printf("onos:%s$ ", directory);
}

static int onos_shell_split(char *line, char **argv)
{
	int argc = 0;
	char *cursor = line;

	while (*cursor != '\0' && argc < ONOS_ARGC_MAX - 1) {
		while (*cursor == ' ' || *cursor == '\t' || *cursor == '\n') {
			cursor++;
		}
		if (*cursor == '\0') {
			break;
		}
		argv[argc++] = cursor;
		while (*cursor != '\0' && *cursor != ' ' && *cursor != '\t' &&
			*cursor != '\n') {
			cursor++;
		}
		if (*cursor != '\0') {
			*cursor++ = '\0';
		}
	}
	argv[argc] = NULL;
	return argc;
}

static int onos_shell_builtin(char **argv, int argc)
{
	struct utsname system_info;

	if (strcmp(argv[0], "help") == 0) {
		puts("builtins: help echo pwd cd clear uname sleep exit");
		return 1;
	}
	if (strcmp(argv[0], "echo") == 0) {
		for (int index = 1; index < argc; index++) {
			if (index > 1) {
				putchar(' ');
			}
			fputs(argv[index], stdout);
		}
		putchar('\n');
		return 1;
	}
	if (strcmp(argv[0], "pwd") == 0) {
		char directory[ONOS_LINE_SIZE];
		if (getcwd(directory, sizeof(directory)) == NULL) {
			perror("pwd");
		} else {
			puts(directory);
		}
		return 1;
	}
	if (strcmp(argv[0], "cd") == 0) {
		const char *directory = argc > 1 ? argv[1] : "/";
		if (chdir(directory) != 0) {
			perror("cd");
		}
		return 1;
	}
	if (strcmp(argv[0], "clear") == 0) {
		fputs("\033[2J\033[H", stdout);
		return 1;
	}
	if (strcmp(argv[0], "uname") == 0) {
		if (uname(&system_info) != 0) {
			perror("uname");
			return 1;
		}
		printf("%s %s %s %s %s\n", system_info.sysname,
			system_info.nodename, system_info.release, system_info.version,
			system_info.machine);
		return 1;
	}
	if (strcmp(argv[0], "exit") == 0) {
		return 2;
	}
	return 0;
}

static void onos_shell_reset_child_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGTSTP, SIG_DFL);
}

static int onos_shell_wait_foreground(pid_t child)
{
	int status;
	pid_t waited;

	do {
		waited = waitpid(child, &status, 0);
		if (waited < 0 && errno != EINTR) {
			return -1;
		}
	} while (waited < 0 || (!WIFEXITED(status) && !WIFSIGNALED(status)));

	if (onos_tty_set_foreground(&shell_tty, getpgrp()) != 0) {
		return -1;
	}
	return status;
}

static int onos_shell_run_sleep(const char *seconds)
{
	char *end;
	long duration = strtol(seconds, &end, 10);
	struct timespec remaining;
	pid_t child;

	if (*seconds == '\0' || *end != '\0' || duration < 0) {
		fputs("sleep: expected a non-negative integer\n", stderr);
		return 1;
	}
	child = fork();
	if (child < 0) {
		return -1;
	}
	if (child == 0) {
		onos_shell_reset_child_signals();
		remaining.tv_sec = duration;
		remaining.tv_nsec = 0;
		while (nanosleep(&remaining, &remaining) != 0 && errno == EINTR) {
		}
		_exit(0);
	}
	setpgid(child, child);
	onos_tty_set_foreground(&shell_tty, child);
	return onos_shell_wait_foreground(child);
}

static int onos_shell_run_program(char **argv)
{
	pid_t child = fork();
	if (child < 0) {
		return -1;
	}
	if (child == 0) {
		onos_shell_reset_child_signals();
		setpgid(0, 0);
		execvp(argv[0], argv);
		dprintf(STDERR_FILENO, "%s: %s\n", argv[0], strerror(errno));
		_exit(127);
	}

	setpgid(child, child);
	if (onos_tty_set_foreground(&shell_tty, child) != 0) {
		return -1;
	}
	return onos_shell_wait_foreground(child);
}

int main(void)
{
	char line[ONOS_LINE_SIZE];
	char *argv[ONOS_ARGC_MAX];
	int result;

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	setenv("PATH", "/bin:/sbin:/usr/bin:/usr/sbin", 1);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);

	if (onos_tty_init(&shell_tty, STDIN_FILENO) != 0 ||
		onos_tty_set_foreground(&shell_tty, getpgrp()) != 0) {
		perror("[ONOS] TTY setup");
		return EXIT_FAILURE;
	}

	for (;;) {
		onos_shell_print_prompt();
		ssize_t length = onos_tty_read(&shell_tty, line, sizeof(line) - 1);
		if (length == 0) {
			putchar('\n');
			break;
		}
		if (length < 0) {
			perror("[ONOS] terminal read");
			break;
		}
		line[length] = '\0';
		int argc = onos_shell_split(line, argv);
		if (argc == 0) {
			continue;
		}

		result = onos_shell_builtin(argv, argc);
		if (result == 2) {
			break;
		}
		if (result == 1) {
			continue;
		}
		if (strcmp(argv[0], "sleep") == 0) {
			if (argc != 2) {
				fputs("sleep: usage: sleep SECONDS\n", stderr);
				continue;
			}
			if (onos_shell_run_sleep(argv[1]) < 0) {
				perror("sleep");
			}
			continue;
		}
		if (onos_shell_run_program(argv) < 0) {
			perror("[ONOS] foreground process");
		}
	}

	onos_tty_restore(&shell_tty);
	return EXIT_SUCCESS;
}
