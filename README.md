Overview
========

`libdjinterop` is a C++ library that allows access to database formats used to
store information about DJ record libraries.

State of Support
================

The library is currently in development, and not all features are implemented
yet.  It supports the Engine Library format, and can read the AlphaTheta
OneLibrary format.

What is supported:

* Track metadata
* Beat grids
* Hot cues
* Loops
* Waveforms
* Crates
* Playlists

The library supports the following firmware and application versions:

* Engine DJ OS from 1.0.3 to 4.3.3.
  * Tested on Denon SC5000 and Numark Mixstream Pro. Other players (e.g.
    SC6000/M) may work, but this is currently untested.
* Engine DJ Desktop (aka Engine Prime) from 1.0.1 to 4.3.0.

The library also reads the AlphaTheta OneLibrary format, which rekordbox 7
writes to USB media as `PIONEER/rekordbox/exportLibrary.db`, and which players
from the CDJ-3000X, XDJ-AZ, OPUS-QUAD and OMNIS-DUO onwards read, as does the
CDJ-3000 from firmware 3.15.  Track metadata, playlists and crates can be read.
Writing is not supported yet, and beat grids, waveforms, hot cues and loops are
not held in the database at all, as rekordbox leaves them in the ANLZ files
beside it.  The format is also documented under the name Device Library Plus.

What is not supported (yet):

* Album art
* Play history
* Writing OneLibrary databases
* DJ record libraries in formats other than Engine Prime and OneLibrary

How Do I Use It?
================

If you are new to the library, see the [introductory guide](GUIDE.md) for an
explanation of how the library is structured and how to get started.

The [examples/](example) directory contains some small self-contained
applications that use the library.

Detailed reference documentation for the public API can be viewed on
[GitHub Pages](https://xsco.github.io/libdjinterop/).

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
$ mkdir cmake-build
$ cd cmake-build
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
