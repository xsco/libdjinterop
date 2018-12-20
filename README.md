Overview
========

`libdjinterop` is a C++ library that allows access to database formats used to store information about DJ record libraries.

This library currently supports:

* Engine Library, as used on "Prime"-series DJ equipment.

State of Support
================

The library is currently in an early alpha stage, and is almost certainly not usable for Production systems just yet.  It currently supports only the Engine Library format.

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
#include <chrono>
#include <iostream>
#include <djinterop/enginelibrary.hpp>

namespace c = std::chrono;
namespace el = djinterop::enginelibrary;

int main(int argc, char **argv)
{
    auto db_dir = "Engine Library";
    auto db = el::create_database(db_dir, el::version_1_7_1);

    // Set track data
    el::track t;
    t.set_track_number(1);
    t.set_duration(std::chrono::seconds{366});
    t.set_bpm(120);
    t.set_year(1970);
    t.set_title("Some Song");
    t.set_artist("Some Artist");
    t.set_key(el::musical_key::a_minor);
    t.set_path("../01 - Some Artist - Some Song.mp3");
    t.set_filename("01 - Some Artist - Some Song.mp3");
    t.set_file_extension("mp3");
    t.set_bitrate(320);
    t.save(db);

    // Set core performance data
    el::performance_data p{t.id()};
    p.set_sample_rate(44100);
    p.set_total_samples(16140600);
    p.set_key(el::musical_key::a_minor);
    p.set_average_loudness(0.5);

    // Set beat grid
    p.set_default_beat_grid(el::track_beat_grid{
            -4,
            -83316.78,
            812,
            17470734.439});

    // Set cues
    std::vector<el::track_hot_cue_point> cues;
    cues.emplace_back(true, "Cue 1", 1377924.5, el::standard_pad_colours::pad_1);
    cues.emplace_back();
    cues.emplace_back(true, "Cue 3", 5508265.964, el::standard_pad_colours::pad_3);
    p.set_hot_cues(std::begin(cues), std::end(cues));
    p.set_adjusted_main_cue_sample_offset(2732);
    p.set_default_main_cue_sample_offset(2732);

    // Set loops
    std::vector<el::track_loop> loops;
    loops.emplace_back(
            true, true, "Loop 1",
            1144.012, 345339.134, el::standard_pad_colours::pad_1);
    p.set_loops(std::begin(loops), std::end(loops));

    // Set overview waveform
    std::vector<el::overview_waveform_entry> ov_waveform_entries;
    uint_least64_t ov_adjusted_total_samples;
    uint_least64_t ov_num_entries;
    double ov_samples_per_entry;
    el::calculate_overview_waveform_details(
            p.total_samples(),
            p.sample_rate(),
            ov_adjusted_total_samples,
            ov_num_entries,
            ov_samples_per_entry);
    for (auto i = 0; i < ov_num_entries; ++i)
    {
        ov_waveform_entries.emplace_back(
                i * 255 / ov_num_entries,
                i * 127 / ov_num_entries,
                i * 63  / ov_num_entries);
    }
    p.set_overview_waveform_entries(
            ov_num_entries,
            ov_samples_per_entry,
            std::begin(ov_waveform_entries),
            std::end(ov_waveform_entries));

    // Set high-resolution waveform
    std::vector<el::high_res_waveform_entry> hr_waveform_entries;
    uint_least64_t hr_adjusted_total_samples;
    uint_least64_t hr_num_entries;
    double hr_samples_per_entry;
    el::calculate_high_res_waveform_details(
            p.total_samples(),
            p.sample_rate(),
            hr_adjusted_total_samples,
            hr_num_entries,
            hr_samples_per_entry);
    for (auto i = 0; i < hr_num_entries; ++i)
    {
        hr_waveform_entries.emplace_back(
                255,
                (i * 127 / hr_num_entries) + 128,
                i * 255 / hr_num_entries,
                i * 255 / hr_num_entries,
                i * 255 / hr_num_entries,
                i * 255 / hr_num_entries);
    }
    p.set_high_res_waveform_entries(
            hr_num_entries,
            hr_samples_per_entry,
            std::begin(hr_waveform_entries),
            std::end(hr_waveform_entries));
    p.save(db);
    
    el::crate cr;
    cr.set_name("My Example Crate");
    cr.add_track(t.id());
    cr.save(db);

    return 0;
}
```

How Do I Build It?
============================

`libdjinterop` requires headers to be installed for:

* [zlib](http://zlib.net)
* [Boost](https://boost.org) (only for building unit tests; not needed for the main library)

Assuming you have the above dependencies in place, and the build tools, you can issue the following commands:

```
$ ./autogen.sh
$ make
$ make check               (optional, run unit tests)
# make install
```

The `autogen.sh` script is a wrapper around `configure`, so you can pass parameters into it as you desire, such as:

```
$ ./autogen.sh --disable-shared --prefix=/usr
...
```

Thanks To
=========

`libdjinterop` makes use of a number of software libraries, and is extremely grateful for:

* [Boost](https://boost.org)
* [SQLite](https://sqlite.org)
* [SQLite Modern C++ Wrapper](https://github.com/SqliteModernCpp/sqlite_modern_cpp)
* [zlib](http://zlib.net)

Interfacing with the Engine Library database format was made a lot easier with the help of MixMasterG from ATGR, who is the author of the [Denon Conversion Utility](https://sellfy.com/atgr_production_team).
