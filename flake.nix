{
	description = "ONoS development environment";

	inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

	outputs = { self, nixpkgs }:
		let
			systems = [ "aarch64-darwin" "aarch64-linux" "x86_64-linux" ];
			forAllSystems = function:
				nixpkgs.lib.genAttrs systems (system: function (import nixpkgs {
					inherit system;
					config.allowUnsupportedSystem = true;
				}));
		in {
			devShells = forAllSystems (pkgs: {
				default = pkgs.mkShell {
					packages = with pkgs; [
						gcc
						clang
						meson
						ninja
						pkg-config
						binutils
						cpio
						gzip
						qemu
						OVMF
						xorriso
						dosfstools
						mtools
						gdb
					] ++ pkgs.lib.optionals pkgs.stdenv.hostPlatform.isLinux [
						pkgs.grub2
						pkgs.pkgsCross.gnu64.linuxPackages.kernel
					];

					shellHook = pkgs.lib.optionalString pkgs.stdenv.hostPlatform.isLinux ''
						export ONOS_KERNEL="${pkgs.pkgsCross.gnu64.linuxPackages.kernel}/bzImage"
						export ONOS_BUSYBOX="${pkgs.pkgsCross.gnu64.busybox.override { enableStatic = true; }}/bin/busybox"
					'' + ''
						export ONOS_BUSYBOX="${pkgs.pkgsCross.gnu64.busybox.override { enableStatic = true; }}/bin/busybox"
					'';
				};
			});
		};
}
