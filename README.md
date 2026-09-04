# ONoS

ONoS (One Night of Sin) is an experimental POSIX amnesic operating
system. Its guiding principle is simple: runtime changes should disappear on
shutdown unless the user explicitly chooses persistence.

This repository is at the first userspace boot-test stage. It boots a static
ONoS init and shell with an external Linux kernel. It does **not** yet contain
a custom UEFI loader or an amnesic overlay filesystem.

### Current state

- The project has a Meson build and a reproducible Nix development shell.
- `init/` contains a small host-buildable C scaffold for the future `/init`.
- `image/` builds a native ONoS initramfs.
- Linux hosts with GRUB can also build a BIOS bootable ISO.

### Development

Enter the development environment:

```sh
nix develop
```

Configure and build with Meson:

```sh
meson setup build
meson compile -C build
./build/init/onos-init
```

On macOS, configure the Linux userspace cross build explicitly:

```sh
meson setup build --cross-file cross/x86_64-linux.ini
meson compile -C build
```

After changing Meson files, refresh an existing build directory:

```sh
meson setup build --reconfigure
```

The build produces a static x86_64 Linux `onos-init` and `onos-shell`. The
initramfs uses `onos-init` as PID 1 and does not require BusyBox for init,
console, TTY, session, or shell handling.

Build the initramfs:

```sh
meson compile -C build onos-initramfs
```

On a Linux development host, build and boot the BIOS ISO:

```sh
meson compile -C build onos-image
meson compile -C build run
```

On macOS, provide an existing x86_64 Linux kernel and boot the initramfs
directly with QEMU:

```sh
export ONOS_KERNEL=/path/to/bzImage
meson compile -C build run
```

QEMU uses the serial console. Stop it with `Ctrl-A`, then `X`.
