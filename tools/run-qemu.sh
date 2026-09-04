#!/bin/sh
set -eu

initramfs=$1

if [ -z "${ONOS_KERNEL:-}" ]; then
	printf '%s\n' 'ONoS run: ONOS_KERNEL is not set.' >&2
	printf '%s\n' 'Set it to an x86_64 Linux bzImage, for example:' >&2
	printf '%s\n' '  export ONOS_KERNEL=/path/to/bzImage' >&2
	exit 1
fi

exec qemu-system-x86_64 \
	-kernel "$ONOS_KERNEL" \
	-initrd "$initramfs" \
	-append 'console=ttyS0,115200 rdinit=/init' \
	-serial stdio \
	-display none \
	-no-reboot
