Overview
========

`libdjinterop` is a C++ library that allows access to database formats used to store information about DJ record libraries.

This library currently supports:

* Engine Library, as used on "Prime"-series DJ equipment.

State of Support
================

The library is currently in an early alpha stage, and not all features are implemented yet.  It currently supports only the Engine Library format.

What is supported:

* Track metadata
* Beat grids
* Hot cues
* Loops
* Waveforms (overview and high-resolution)
* Crates
* Engine library formats for the following database schema versions:
  * Schema 1.6.0 (used by firmware 1.0.0)
  * Schema 1.7.1 (used by firmware 1.0.3)

What is not supported (yet):

* Album art
* Playlists
* Play history
* Engine Library formats associated with other firmware versions
* DJ record libraries in formats other than Engine Prime

How Do I Use It?
================

The library is not ready for prime-time yet, but if you are willing to read the source code, you can get an example application up and running using code similar to the following:

```c++
#include <djinterop/djinterop.hpp>

namespace el = djinterop::enginelibrary;

int main(int argc, char **argv)
{
    auto db = el::make_database("Engine Library");

    auto tr = db.create_track("../01 - Some Artist - Some Song.mp3");

    tr.set_track_number(1);
    tr.set_bpm(120);
    tr.set_year(1970);
    tr.set_title("Some Song");
    tr.set_artist("Some Artist");
    tr.set_publisher(boost::none);  // boost::none indicates missing metadata
    tr.set_key(djinterop::musical_key::a_minor);
    tr.set_bitrate(320);
    tr.set_average_loudness(0.5);  // loudness range (0, 1]
    tr.set_sampling({44100,        // sample rate
                     16140600});   // sample count
    std::vector<djinterop::beatgrid_marker> beatgrid{
        {-4, -83316.78},                 // 1st marker
        {812, 17470734.439}};            // 2nd marker
    tr.set_default_beatgrid(beatgrid);   // as analyzed
    tr.set_adjusted_beatgrid(beatgrid);  // manually adjusted

    // The main cue concerns the cue button
    tr.set_default_main_cue(2732);   // as analyzed
    tr.set_adjusted_main_cue(2732);  // manually adjusted

    // There are always 8 hot cues, whereby each can optionally be set
    std::array<boost::optional<djinterop::hot_cue>, 8> cues;
    cues[0] = djinterop::hot_cue{"Cue 1", 1377924.5,  // position in number of samples
                          djinterop::standard_pad_colors::pad_1};
    tr.set_hot_cues(cues);

    // Setting a single hot cue can also be done like this
    tr.set_hot_cue_at(3, {"Cue 4", 5508265.96, djinterop::standard_pad_colors::pad_4});

    // The loop API works like the hot cue API
    tr.set_loop_at(
        0, {"Loop 1", 1144.012, 345339.134, djinterop::standard_pad_colors::pad_1});

    // Set high-resolution waveform
    int64_t waveform_size = tr.recommended_waveform_size();
    std::vector<djinterop::waveform_entry> waveform;
    waveform.reserve(waveform_size);
    for (int64_t i = 0; i < waveform_size; ++i)
    {
        waveform.push_back(  // VALUE and OPACITY for each band (low/mid/high)
            {{0, 255},       // low
             {42, 255},      // mid
             {255, 255}});   // high
    }
    tr.set_waveform(std::move(waveform));

    auto cr = db.create_crate("My Example Crate");
    cr.add_track(tr);
}
```

How Do I Build It?
============================

`libdjinterop` requires the following compile-time dependencies:

* [zlib](http://zlib.net)
* [Boost](https://boost.org) (libraries required for unit tests; headers only for the main library)

`libdjinterop` uses the [Meson build system](https://mesonbuild.com).  Assuming you have the above dependencies in place, and the build tools, you can issue the following commands:

```
$ meson build/
$ ninja -C build/
$ ninja -C build/ test                (optional, run unit tests)
# ninja -C build/ install             (as a suitably-privileged user)
```

## With Nix

When [Nix](http://nixos.org/nix) is installed, then you don't need to manually install any
dependencies.
`libdjinterop` can then simply be built with:

```
$ nix build
```

In order to drop into a development environment with dependencies available, execute:

```
$ nix-shell
```

You can then build `libdjinterop` by using Meson as described above.
This is advantageous when developing since it only recompiles sources that it needs to.

Thanks To
=========

`libdjinterop` makes use of a number of software libraries, and is extremely grateful for:

* [Boost](https://boost.org)
* [SQLite](https://sqlite.org)
* [SQLite Modern C++ Wrapper](https://github.com/SqliteModernCpp/sqlite_modern_cpp)
* [zlib](http://zlib.net)
* [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html)

Interfacing with the Engine Library database format was made a lot easier with the help of MixMasterG from ATGR, who is the author of the [Denon Conversion Utility](https://sellfy.com/atgr_production_team).
