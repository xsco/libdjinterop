with import <nixpkgs> {};

stdenv.mkDerivation {
  name = "libdjinterop";
  version = "2019-06-05";
  src = builtins.fetchGit ./.;
  nativeBuildInputs = [
    meson
    ninja
    pkg-config
  ];
  outputs = [ "out" "dev" ];
  buildInputs = [
    zlib
    boost
  ];
}
