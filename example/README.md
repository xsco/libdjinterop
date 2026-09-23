Overview
========

This directory contains small example applications that illustrate the use
of `libdjinterop`.

| Example                       | What it does                                                        |
|-------------------------------|---------------------------------------------------------------------|
| `engine_prime`                | Writes a track, a crate and a playlist to an Engine Prime library.  |
| `engine_library_v2_low_level` | Uses the low-level Engine v2 API to work with tables directly.      |
| `onelibrary`                  | Prints the tracks and playlists of an AlphaTheta OneLibrary device. |

They are built by the project itself, as `example_engine_prime` and so on,
when it is configured with `-DBUILD_EXAMPLES=ON`.

To build one of them against an installed `libdjinterop` instead, as a
starting point for a program of your own:

```shell
g++ -std=c++20 `pkg-config --cflags djinterop` onelibrary.cpp `pkg-config --libs djinterop` -o onelibrary
```

The library needs a C++20 compiler, and its headers refuse to compile under
any older standard.

`onelibrary` takes the device to read as its argument, either the root
directory of a device or the `exportLibrary.db` file itself, and optionally a
passphrase:

```shell
./onelibrary /Volumes/MYUSB
```
