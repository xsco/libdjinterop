Overview
========

`libdjinterop` is a C++ library that allows access to database formats used to
store information about DJ record libraries.

This library currently supports:

* Engine Library, as used on "Prime"-series DJ equipment.

Documentation can be viewed on [GitHub Pages](https://xcso.github.io/libdjinterop/)

State of Support
================

The library is currently in an early beta stage, and not all features are
implemented yet.  It currently supports only the Engine Library format.

What is supported:

* Track metadata
* Beat grids
* Hot cues
* Loops
* Waveforms (overview and high-resolution)
* Crates

The library supports the following firmware and application versions:

* SC5000 Firmware from 1.0.3 to 3.4.0.
  * Other players (e.g. SC6000/M) may work, but this is currently untested.
* Engine DJ Desktop (aka Engine Prime) from 1.0.1 to 3.4.0.

What is not supported (yet):

* Album art
* Playlists
* Play history
* Engine Library formats associated with other firmware versions
* DJ record libraries in formats other than Engine Prime

How Do I Use It?
================

The library is not ready for prime-time yet, but if you are willing to read
source code, example applications can be found in the `example` directory.

How Do I Build It?
============================

`libdjinterop` requires the following compile-time dependencies:

* [SQLite3](https://sqlite.org)
* [zlib](http://zlib.net)

To run unit tests, the following are required:

* [Boost](https://boost.org)

`libdjinterop` uses [CMake](https://cmake.org/).  Assuming you have the above
dependencies in place, and the build tools, you can issue the following
commands:

```shell
$ mkdir cmake_build
$ cd cmake_build
$ cmake ..
$ cmake --build .
$ ctest   # To run unit tests
```

## With Nix

When [Nix](http://nixos.org/nix) is installed, then you don't need to manually
install any dependencies.  In order to drop into a development environment with
dependencies available, execute:

```shell
$ nix-shell
```

You can then build `libdjinterop` by as described above.

Thanks To
=========

`libdjinterop` makes use of a number of software libraries, and is extremely
grateful for:

* [SQLite](https://sqlite.org)
* [SQLite Modern C++ Wrapper](https://github.com/SqliteModernCpp/sqlite_modern_cpp)
* [zlib](http://zlib.net)
* [Boost](https://boost.org)
* [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html)

Interfacing with the Engine Library database format was made a lot easier with
the help of MixMasterG from ATGR, who is the author of the
[Denon Conversion Utility](https://sellfy.com/atgr_production_team).
