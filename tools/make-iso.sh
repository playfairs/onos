#!/bin/sh
set -eu

initramfs=$1
output=$2

grub_mkrescue=$(command -v grub2-mkrescue || command -v grub-mkrescue || true)
if [ -z "$grub_mkrescue" ]; then
    printf '%s\n' 'ONoS image: grub2-mkrescue is unavailable on this host.' >&2
    printf '%s\n' 'Build the ISO on Linux, or use the direct QEMU run target on macOS.' >&2
    exit 1
fi

if [ -z "${ONOS_KERNEL:-}" ]; then
    printf '%s\n' 'ONoS image: ONOS_KERNEL is not set.' >&2
    printf '%s\n' 'Set it to an x86_64 Linux bzImage before building the ISO.' >&2
    exit 1
fi

root=$(mktemp -d)
trap 'rm -rf "$root"' EXIT
mkdir -p "$root/boot/grub"
cp "$ONOS_KERNEL" "$root/boot/vmlinuz"
cp "$initramfs" "$root/boot/onos-initramfs.cpio.gz"

cat > "$root/boot/grub/grub.cfg" <<'GRUB'
set timeout=0
set default=0

menuentry 'ONoS development image' {
    linux /boot/vmlinuz console=tty0 rdinit=/init
    initrd /boot/onos-initramfs.cpio.gz
}
GRUB

"$grub_mkrescue" -o "$output" "$root"
