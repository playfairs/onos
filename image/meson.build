sh = find_program('sh')

initramfs = custom_target(
	'onos-initramfs',
	input: [onos_init, onos_shell],
		output: 'onos-initramfs.cpio.gz',
		command: [
			sh,
			meson.project_source_root() / 'tools/make-initramfs.sh',
			'@INPUT0@',
			'@INPUT1@',
			'@OUTPUT@',
		],
		depend_files: files('../tools/make-initramfs.sh'),
		build_by_default: true,
)

run_target(
	'run',
		command: [
			sh,
			meson.project_source_root() / 'tools/run-qemu.sh',
			initramfs,
		],
		depends: initramfs,
)

custom_target(
	'onos-image',
	input: initramfs,
	output: 'onos.iso',
	command: [
		sh,
		meson.project_source_root() / 'tools/make-iso.sh',
		'@INPUT@',
		'@OUTPUT@',
	],
	depend_files: files('../tools/make-iso.sh'),
	build_always_stale: true,
)

message('ONoS image: target registered; ISO creation requires Linux, GRUB, and ONOS_KERNEL')
