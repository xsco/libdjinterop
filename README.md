Overview
========

`libdjinterop` is a C++ library that allows access to database formats used to
store information about DJ record libraries.

This library currently supports:

* Engine Library, as used on "Prime"-series DJ equipment.

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

* SC5000 Firmware 1.0.3, 1.2.0, 1.2.2, 1.3.1, 1.4.0, 1.5.1, 1.5.2.
  * Other players (e.g. SC6000/M) may work, but this is currently untested.
* Engine Prime 1.0.1, 1.2.2, 1.5.1.

What is not supported (yet):

* Album art
* Playlists
* Play history
* Engine Library formats associated with other firmware versions
* DJ record libraries in formats other than Engine Prime

How Do I Use It?
================

The library is not ready for prime-time yet, but if you are willing to read
source code, an example application can be found in the [example](example)
directory.

How Do I Build It?
============================

`libdjinterop` requires the following compile-time dependencies:

* [SQLite3](https://sqlite.org)
* [zlib](http://zlib.net)
* [Boost](https://boost.org) (only needed for unit tests, not the main library)

`libdjinterop` uses the [Meson build system](https://mesonbuild.com).  Assuming
you have the above dependencies in place, and the build tools, you can issue
the following commands:

```
$ meson build/
$ ninja -C build/
$ ninja -C build/ test                (optional, run unit tests)
# ninja -C build/ install             (as a suitably-privileged user)
```

## With Nix

When [Nix](http://nixos.org/nix) is installed, then you don't need to manually
install any dependencies.  `libdjinterop` can then simply be built with:

```
$ nix build
```

In order to drop into a development environment with dependencies available,
execute:

```
$ nix-shell
```

You can then build `libdjinterop` by using Meson as described above.

This is advantageous when developing since it only recompiles sources that it
needs to.

Thanks To
=========

`libdjinterop` makes use of a number of software libraries, and is extremely
grateful for:

* [Boost](https://boost.org)
* [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html)
* [SQLite](https://sqlite.org)
* [SQLite Modern C++ Wrapper](https://github.com/SqliteModernCpp/sqlite_modern_cpp)
* [zlib](http://zlib.net)

Interfacing with the Engine Library database format was made a lot easier with
the help of MixMasterG from ATGR, who is the author of the
[Denon Conversion Utility](https://sellfy.com/atgr_production_team).
