{ pkgs ? import <nixpkgs> {} }:

let
  capnproto = pkgs.capnproto.overrideAttrs (oldAttrs: rec {
    name = "capnproto-${version}";
    version = "0.6.0";
    src = pkgs.fetchurl {
      url = "https://capnproto.org/capnproto-c++-${version}.tar.gz";
      sha256 = "0gpp1cxsb9nfd7qkjjykzknx03y0z0n4bq5q0fmxci7w38ci22g5";
    };
    enableParallelBuilding = true;
    nativeBuildInputs = with pkgs; [ cmake extra-cmake-modules ];
  });
in
pkgs.stdenv.mkDerivation rec {
  name = "wzNet-pg-${version}";
  version = "0.0.1.0";
  nativeBuildInputs = with pkgs; [ cmake gnumake gcc ];
  buildInputs = [
    capnproto
  ];
  shellHook = ''
    echo "Build project with:"
    echo "cmake -H. -Bbuild && make -C build"
    PS1='wzNet-pg$ '
  '';
}
