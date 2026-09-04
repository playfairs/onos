#include "rootfs.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#define ONOS_MAKEDEV(major, minor) \
	((dev_t)((((dev_t)(major) & 0xfff) << 8) | \
	((dev_t)(minor) & 0xff) | (((dev_t)(minor) & ~0xff) << 12)))

int onos_prepare_root_filesystem(void)
{
	if (mknod("/dev/console", S_IFCHR | 0600, ONOS_MAKEDEV(5, 1)) == 0 ||
		errno == EEXIST) {
		return 0;
	}
	return -1;
}
