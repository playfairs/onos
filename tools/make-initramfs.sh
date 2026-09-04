#!/bin/sh
set -eu

busybox=$1
output=$2
root=$(mktemp -d)
trap 'rm -rf "$root"' EXIT
mkdir -p "$(dirname "$output")"
output_dir=$(cd "$(dirname "$output")" && pwd)
output="$output_dir/$(basename "$output")"

mkdir -p "$root/bin" "$root/dev" "$root/proc" "$root/sys"
cp "$busybox" "$root/bin/busybox"
ln -s busybox "$root/bin/sh"
ln -s busybox "$root/bin/setsid"
ln -s busybox "$root/bin/cttyhack"

cat > "$root/init" <<'INIT'
#!/bin/sh
mount -t proc proc /proc 2>/dev/null || true
mount -t sysfs sysfs /sys 2>/dev/null || true
mount -t devtmpfs devtmpfs /dev 2>/dev/null || true

echo "ONoS booted successfully"
echo "This is the temporary BusyBox initramfs milestone."

exec /bin/setsid /bin/cttyhack /bin/sh
INIT
chmod 0755 "$root/init"

(cd "$root" && find . -print | cpio -o -H newc | gzip -9 > "$output")
