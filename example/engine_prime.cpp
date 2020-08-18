/*
    This file is part of libdjinterop.

    libdjinterop is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libdjinterop is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libdjinterop.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <djinterop/djinterop.hpp>

namespace el = djinterop::enginelibrary;

int main(int argc, char** argv)
{
    using namespace std::string_literals;

    auto dir = "Engine Library"s;
    bool created;
    auto db = el::create_or_load_database(dir, el::version_latest, created);
    std::cout << (created ? "Created " : "Loaded ") << "database in directory "
              << dir << std::endl;

    for (auto& cr : db.crates())
    {
        std::cout << "Removing prior crate " << cr.name() << std::endl;
        db.remove_crate(cr);
    }

    for (auto& tr : db.tracks())
    {
        std::cout << "Removing prior track " << tr.filename() << std::endl;
        db.remove_track(tr);
    }

    auto tr = db.create_track("../01 - Some Artist - Some Song.mp3");
    std::cout << "Added track " << tr.filename() << std::endl;

    tr.set_track_number(1);
    tr.set_bpm(120);
    tr.set_year(1970);
    tr.set_title("Some Song"s);
    tr.set_artist("Some Artist"s);
    tr.set_publisher(djinterop::stdx::nullopt);  // indicates missing metadata
    tr.set_key(djinterop::musical_key::a_minor);
    tr.set_bitrate(320);
    tr.set_average_loudness(0.5);  // loudness range (0, 1]
    int64_t sample_count = 16140600;
    tr.set_sampling(
        {44100,           // sample rate
         sample_count});  // sample count
    std::vector<djinterop::beatgrid_marker> beatgrid{
        {-4, -83316.78},                 // 1st marker
        {812, 17470734.439}};            // 2nd marker
    tr.set_default_beatgrid(beatgrid);   // as analyzed
    tr.set_adjusted_beatgrid(beatgrid);  // manually adjusted

    // The main cue concerns the cue button
    tr.set_default_main_cue(2732);   // as analyzed
    tr.set_adjusted_main_cue(2732);  // manually adjusted

    // There are always 8 hot cues, whereby each can optionally be set
    std::array<djinterop::stdx::optional<djinterop::hot_cue>, 8> cues;
    cues[0] = djinterop::hot_cue{
        "Cue 1", 1377924.5,  // position in number of samples
        el::standard_pad_colors::pad_1};
    tr.set_hot_cues(cues);

    // Setting a single hot cue can also be done like this
    tr.set_hot_cue_at(3, {"Cue 4", 5508265.96, el::standard_pad_colors::pad_4});

    // The loop API works like the hot cue API
    tr.set_loop_at(
        0, {"Loop 1", 1144.012, 345339.134, el::standard_pad_colors::pad_1});

    // Set high-resolution waveform
    int64_t spe = tr.required_waveform_samples_per_entry();
    int64_t waveform_size = (sample_count + spe - 1) / spe;  // Ceiling division
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

    auto cr = db.create_root_crate("My Example Crate");
    cr.add_track(tr);
    std::cout << "Added track to crate " << cr.name() << std::endl;
}
