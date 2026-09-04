init_sources = files(
	'main.c',
	'mounts.c',
	'rootfs.c',
	'console.c',
	'tty.c',
	'session.c',
)

onos_build_id = meson.project_version()
onos_build_define = '-DONOS_BUILD_ID="' + onos_build_id + '"'

onos_init = executable(
	'onos-init',
	init_sources,
	c_args: ['-D_GNU_SOURCE', onos_build_define, '-O2', '-static'],
	link_args: ['-static'],
	install: false,
)

onos_shell = executable(
	'onos-shell',
	files('shell.c', 'console.c', 'tty.c'),
	c_args: ['-D_GNU_SOURCE', onos_build_define, '-O2', '-static'],
	link_args: ['-static'],
	install: false,
)
