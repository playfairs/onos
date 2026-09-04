#!/bin/sh
set -eu

onos_init=$1
onos_shell=$2
output=$3
root=$(mktemp -d)
trap 'rm -rf "$root"' EXIT
mkdir -p "$(dirname "$output")"
output_dir=$(cd "$(dirname "$output")" && pwd)
output="$output_dir/$(basename "$output")"

mkdir -p "$root/bin" "$root/dev" "$root/proc" "$root/sys"
cp "$onos_init" "$root/init"
cp "$onos_shell" "$root/bin/onos-shell"
chmod 0755 "$root/init" "$root/bin/onos-shell"

(cd "$root" && find . -print | cpio -o -H newc | gzip -9 > "$output")
