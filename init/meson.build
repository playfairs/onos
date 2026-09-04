init_sources = files(
	'main.c',
	'mounts.c',
	'rootfs.c',
	'console.c',
	'tty.c',
	'session.c',
)

onos_init = executable(
	'onos-init',
	init_sources,
	c_args: ['-D_GNU_SOURCE', '-O2', '-static'],
	link_args: ['-static'],
	install: false,
)

onos_shell = executable(
	'onos-shell',
	files('shell.c', 'console.c', 'tty.c'),
	c_args: ['-D_GNU_SOURCE', '-O2', '-static'],
	link_args: ['-static'],
	install: false,
)
