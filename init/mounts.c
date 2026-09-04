#include "mounts.h"

#include <errno.h>
#include <stdio.h>
#if defined(__linux__)
#include <sys/mount.h>
#endif

static int onos_mount(const char *source, const char *target, const char *type)
{
#if defined(__linux__)
	if (mount(source, target, type, 0, NULL) == 0 || errno == EBUSY) {
		return 0;
	}
	return -1;
#else
	(void)source;
	(void)target;
	(void)type;
	errno = ENOSYS;
	return -1;
#endif
}

int onos_mount_virtual_filesystems(void)
{
	puts("[ONOS] mounting proc");
	if (onos_mount("proc", "/proc", "proc") != 0) {
		perror("[ONOS] mount proc");
		return -1;
	}

	puts("[ONOS] mounting sysfs");
	if (onos_mount("sysfs", "/sys", "sysfs") != 0) {
		perror("[ONOS] mount sysfs");
		return -1;
	}

	puts("[ONOS] mounting devtmpfs");
	if (onos_mount("devtmpfs", "/dev", "devtmpfs") != 0) {
		perror("[ONOS] mount devtmpfs");
		return -1;
	}
	return 0;
}
