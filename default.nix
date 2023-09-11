{ lib, stdenv, fuse, pkg-config }:

stdenv.mkDerivation {
  name = "dffs";
  src = lib.cleanSource ./.;

  nativeBuildInputs = [ pkg-config ];
  buildInputs = [ fuse ];

  installPhase = ''
    mkdir -p $out/bin
    cp dffs $out/bin
  '';
}
