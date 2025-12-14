{
  lib,
  stdenv,
  cmake,
  ninja,
  mold-wrapped,
  doxygen,
  zlib,
  sqlite,
  boost,
  enableTests ? true
}:
stdenv.mkDerivation {
  pname = "libdjinterop";
  version = "unstable";

  src = lib.sourceByRegex ./. [
    "^example.*"
    "^ext.*"
    "^include.*"
    "^src.*"
    "^test.*"
    "^testdata.*"
    "LICENSE"
    "CMakeLists.txt"
    "DjInteropConfig.cmake.in"
    "djinterop.pc.in"
  ];

  # Required at compile time.
  nativeBuildInputs = [
    cmake
    ninja
    mold-wrapped
    doxygen
  ];

  # Required at run time.
  buildInputs = [
    sqlite
    zlib
  ];

  # Required for tests.
  checkInputs = [
    boost
  ];

  doCheck = enableTests;
  cmakeFlags = lib.optional (!enableTests) "-DTESTING=OFF";
}
