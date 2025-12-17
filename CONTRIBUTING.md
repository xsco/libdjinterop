Contributing to libdjinterop
============================

If you are interested in contributing to `libdjinterop`, thank you!

The easiest way to contribute is to look at the
[issues list](https://github.com/xsco/libdjinterop/issues) and see if there is
anything that you might like to try.

Although `libdjinterop` is a C++ library, there are usually both coding and
non-coding tasks available.  All contributions are hugely appreciated!

C++ Style Guide
---------------

C++ coding conventions in `libdjinterop` broadly follow the
[Core C++ Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
from ISO C++.

Furthermore, there is a [.clang-format](.clang-format) file in the repository,
and all code contributions should conform to this.

Perhaps one day there will be more opinionated formatting/linting, or indeed
a pre-commit hook to enforce it.

Unit Testing
------------

High unit test coverage is _strongly encouraged_ for all C++ code contributions.
It may become a mandatory requirement one day.

External Dependencies
---------------------

Introducing any additional external dependencies into `libdjinterop` is a big
decision that requires proper consideration.  The aim is for the library to be
built and compiled with minimal additional requirements, in order to make
wider adoption as easy as possible.

For an external dependency to be considered, ideally it should be readily
available on all stable versions of each target operating system, as well as
be a stable and mature library.

For dependencies that do not meet these criteria, another option is to 'vendor'
the dependencies into the codebase under the [ext/](ext) directory hierarchy.

OS Compatibility
----------------

The library is expected to compile on:

* Windows - all supported versions
* Linux:
  * Ubuntu - all LTS versions
  * Red Hat and friends - all supported versions
  * Arch Linux
* macOS - all recent versions

The biggest user of this library is undoubtedly the [Mixxx](https://mixxx.org)
open source DJ application.  As such, it is a very sensible idea to ensure that
the library always compiles on the operating systems that Mixxx targets too.
