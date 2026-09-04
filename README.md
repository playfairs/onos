# ONoS

ONoS (One Night of Sin) is an experimental POSIX amnesic operating
system. Its guiding principle is simple: runtime changes should disappear on
shutdown unless the user explicitly chooses persistence.

This repository is at the first boot-test stage. It contains a temporary
BusyBox initramfs that can be booted with a Linux kernel. It does **not** yet
contain the C `/init` in that initramfs, a custom UEFI loader, or an amnesic
overlay filesystem.

### Current state

- The project has a Meson build and a reproducible Nix development shell.
- `init/` contains a small host-buildable C scaffold for the future `/init`.
- `image/` builds a temporary BusyBox initramfs.
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

After changing Meson files, refresh an existing build directory:

```sh
meson setup build --reconfigure
```

The current executable only reports which init responsibilities are still
stubs. It is not an initramfs `/init` and must not be used as PID 1 yet.

Build the temporary initramfs:

```sh
meson compile -C build onos-initramfs
```

On a Linux development host, build and boot the BIOS ISO:

```sh
meson compile -C build onos-image
meson compile -C build run
```

On macOS, provide an existing x86_64 Linux kernel and boot the same initramfs
directly with QEMU:

```sh
export ONOS_KERNEL=/path/to/bzImage
meson compile -C build run
```

QEMU uses the serial console. Stop it with `Ctrl-A`, then `X`.
