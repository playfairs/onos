#include <stdio.h>

int onos_mount_virtual_filesystems(void);
int onos_prepare_root_filesystem(void);
int onos_start_session(void);

int main(void)
{
	puts("ONoS init stub");
	puts("The real initramfs boot path is not implemented yet.");

	if (onos_mount_virtual_filesystems() != 0 ||
		onos_prepare_root_filesystem() != 0 ||
		onos_start_session() != 0) {
		fputs("ONoS init: a stub component failed\n", stderr);
		return 1;
	}

	return 0;
}
