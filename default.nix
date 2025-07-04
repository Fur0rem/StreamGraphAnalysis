let
	# Pinned nixpkgs, deterministic. Last updated: 2/12/21.
	# pkgs = import (fetchTarball("https://github.com/NixOS/nixpkgs/archive/a58a0b5098f0c2a389ee70eb69422a052982d990.tar.gz")) {};

	# Rolling updates, not deterministic.
	pkgs = import (fetchTarball("channel:nixpkgs-unstable")) {};	

in pkgs.mkShell {
	buildInputs = with pkgs; [ 
		gcc
		coreutils-prefixed
		re2
		clang-tools
		doxygen
		graphviz
	];
	NIX_ENFORCE_NO_NATIVE=1; # -march=native flag
}
