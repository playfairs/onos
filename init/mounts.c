#include "mounts.h"

#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#if defined(__linux__)
#include <sys/vfs.h>
#include <sys/mount.h>
#endif

#define ONOS_PROC_SUPER_MAGIC 0x9fa0
#define ONOS_SYSFS_MAGIC 0x62656572
#define ONOS_TMPFS_MAGIC 0x01021994

static int onos_mount(const char *source, const char *target,
	const char *type, unsigned long expected_magic)
{
#if defined(__linux__)
	struct statfs filesystem;

	if (mount(source, target, type, 0, NULL) != 0 && errno != EBUSY) {
		return -1;
	}
	if (statfs(target, &filesystem) != 0 || filesystem.f_type != expected_magic) {
		errno = ENODEV;
		return -1;
	}
	return 0;
#else
	(void)source;
	(void)target;
	(void)type;
	(void)expected_magic;
	errno = ENOSYS;
	return -1;
#endif
}

static int onos_prepare_mountpoint(const char *path)
{
	struct stat directory;

	if (mkdir(path, 0755) != 0 && errno != EEXIST) {
		return -1;
	}
	if (stat(path, &directory) != 0 || !S_ISDIR(directory.st_mode)) {
		errno = ENOTDIR;
		return -1;
	}
	return 0;
}

int onos_mount_virtual_filesystems(void)
{
	if (onos_prepare_mountpoint("/proc") != 0 || onos_mount(
		"proc", "/proc", "proc", ONOS_PROC_SUPER_MAGIC) != 0) {
		perror("[ONOS] mount proc");
		return -1;
	}
	puts("[ONOS] mounting proc");

	if (onos_prepare_mountpoint("/sys") != 0 || onos_mount(
		"sysfs", "/sys", "sysfs", ONOS_SYSFS_MAGIC) != 0) {
		perror("[ONOS] mount sysfs");
		return -1;
	}
	puts("[ONOS] mounting sysfs");

	if (onos_prepare_mountpoint("/dev") != 0 || onos_mount(
		"devtmpfs", "/dev", "devtmpfs", ONOS_TMPFS_MAGIC) != 0) {
		perror("[ONOS] mount devtmpfs");
		return -1;
	}
	puts("[ONOS] mounting devtmpfs");
	return 0;
}
