Overview
========

`libengineprime` is a C++ library that allows access to the Engine Library, a
repository of metadata and performance data for "Prime"-series DJ equipment.

State of Support
================

The library is currently in a very early alpha version, and is almost certainly
not usable for Production systems just yet.

What is supported:

* Track metadata
* Beat grids
* Hot cues
* Loops
* Engine library formats for the following firmware versions:
  * Firmware 1.0.0 (schema 1.6.0)
  * Firmware 1.0.3 (schema 1.7.1)

What is not supported (yet):

* Album art
* Waveforms (overview and performance)
* Playlists
* Crates
* Play history
* Engine Library formats associated with other firmware versions

How Do I Use It?
================

The library is not ready for prime-time yet, but if you are willing to read the
source code, you can get an example application up and running using code similar
to the following:

```c++
#include <chrono>
#include <iostream>
#include <engineprime/engineprime.hpp>

namespace c = std::chrono;
namespace ep = engineprime;

int main(int argc, char **argv)
{
    auto db_dir = "Engine Library";
    auto db = ep::create_database(db_dir, ep::version_firmware_1_0_3);

    ep::track t;
    t.set_track_number(1);
    t.set_duration(std::chrono::seconds{366});
    t.set_bpm(120);
    t.set_year(1970);
    t.set_title("Some Song");
	t.set_artist("Some Artist");
    t.set_key(ep::musical_key::a_minor);
	t.set_path("../01 - Some Artist - Some Song.mp3");
	t.set_filename("01 - Some Artist - Some Song.mp3");
	t.set_file_extension("mp3");
    t.set_bitrate(320);
    t.save(db);

    ep::performance_data p{t.id()};
    p.set_sample_rate(44100);
    p.set_total_samples(16140600);
    p.set_key(ep::musical_key::a_minor);
    p.set_average_loudness(0.5);
    p.set_default_beat_grid(ep::track_beat_grid{
            -4,
            -83316.78,
            812,
            17470734.439});
    std::vector<ep::track_hot_cue_point> cues;
    cues.emplace_back(true, "Cue 1", 1377924.5, ep::standard_pad_colours::pad_1);
    cues.emplace_back();
    cues.emplace_back(true, "Cue 3", 5508265.964, ep::standard_pad_colours::pad_3);
    p.set_hot_cues(std::begin(cues), std::end(cues));
    p.set_adjusted_main_cue_sample_offset(2732);
    p.set_default_main_cue_sample_offset(2732);
    std::vector<ep::track_loop> loops;
    loops.emplace_back(
            true, true, "Loop 1",
            1144.012, 345339.134, ep::standard_pad_colours::pad_1);
    p.set_loops(std::begin(loops), std::end(loops));
    p.save(db);

    return 0;
}
```

Dependencies
============

`libengineprime` makes use of:

* [Boost](https://boost.org)
* [SQLite](https://sqlite.org)
* [SQLite Modern C++ Wrapper](https://github.com/SqliteModernCpp/sqlite_modern_cpp)

