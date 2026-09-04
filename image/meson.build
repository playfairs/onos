sh = find_program('sh')
busybox = run_command(
	sh,
	'-c',
	'printf %s "$ONOS_BUSYBOX"',
	check: true,
).stdout().strip()

if busybox == ''
	error('ONOS_BUSYBOX is not set; enter the Nix development shell first')
endif

initramfs = custom_target(
	'onos-initramfs',
		output: 'onos-initramfs.cpio.gz',
		command: [
			sh,
			meson.project_source_root() / 'tools/make-initramfs.sh',
			busybox,
			'@OUTPUT@',
		],
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
)

message('ONoS image: target registered; ISO creation requires Linux, GRUB, and ONOS_KERNEL')
