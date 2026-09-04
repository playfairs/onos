#include "rootfs.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#define ONOS_MAKEDEV(major, minor) \
	((dev_t)((((dev_t)(major) & 0xfff) << 8) | \
	((dev_t)(minor) & 0xff) | (((dev_t)(minor) & ~0xff) << 12)))

int onos_prepare_root_filesystem(void)
{
	struct stat console;
	const dev_t expected = ONOS_MAKEDEV(5, 1);

	if (mknod("/dev/console", S_IFCHR | 0600, expected) != 0 &&
		errno != EEXIST) {
		return -1;
	}
	if (stat("/dev/console", &console) != 0 || !S_ISCHR(console.st_mode) ||
		console.st_rdev != expected) {
		errno = ENODEV;
		return -1;
	}
	return 0;
}
