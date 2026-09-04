#!/bin/sh
set -eu

initramfs=$1
output=$2
: "${ONOS_KERNEL:?ONOS_KERNEL must point to an x86_64 Linux kernel}"

if ! command -v grub-mkstandalone >/dev/null 2>&1; then
    printf '%s\n' 'ONoS IMG: grub-mkstandalone is unavailable.' >&2
    exit 1
fi
if ! command -v mcopy >/dev/null 2>&1; then
    printf '%s\n' 'ONoS IMG: mtools is unavailable.' >&2
    exit 1
fi

mkdir -p "$(dirname "$output")"
output_dir=$(cd "$(dirname "$output")" && pwd)
output="$output_dir/$(basename "$output")"

rm -f "$output"
dd if=/dev/zero of="$output" bs=1M count=128 status=none
mkfs.fat -F 32 "$output" >/dev/null

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

grub-mkstandalone \
    -O x86_64-efi \
    -o "$root/BOOTX64.EFI" \
    "boot/grub/grub.cfg=$root/boot/grub/grub.cfg"

mmd -i "$output" ::/EFI ::/EFI/BOOT ::/boot
mcopy -i "$output" "$root/BOOTX64.EFI" ::/EFI/BOOT/BOOTX64.EFI
mcopy -i "$output" "$root/boot/vmlinuz" ::/boot/vmlinuz
mcopy -i "$output" "$root/boot/onos-initramfs.cpio.gz" ::/boot/onos-initramfs.cpio.gz
