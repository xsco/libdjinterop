with import <nixpkgs> {};

let

  git-clang-format = stdenv.mkDerivation {
    name = "git-clang-format";
    version = "2019-06-21";
    src = fetchurl {
      url = "https://raw.githubusercontent.com/llvm-mirror/clang/2bb8e0fe002e8ffaa9ce5fa58034453c94c7e208/tools/clang-format/git-clang-format";
      sha256 = "1kby36i80js6rwi11v3ny4bqsi6i44b9yzs23pdcn9wswffx1nlf";
      executable = true;
    };
    nativeBuildInputs = [
      makeWrapper
    ];
    buildInputs = [
      clang-tools
      python3
    ];
    unpackPhase = ":";
    installPhase = ''
      mkdir -p $out/opt $out/bin
      cp $src $out/opt/git-clang-format
      makeWrapper $out/opt/git-clang-format $out/bin/git-clang-format \
        --add-flags --binary \
        --add-flags ${clang-tools}/bin/clang-format
    '';
  };

in

stdenv.mkDerivation {
  name = "libdjinterop";
  version = "unstable";
  src = nix-gitignore.gitignoreSource [ ".git*" ] ./.;
  nativeBuildInputs = [
    git-clang-format
    meson
    ninja
    pkg-config
  ];
  outputs = [ "out" "dev" ];
  buildInputs = [
    boost
    zlib
  ];
}
