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

// Print the contents of an AlphaTheta OneLibrary device.
//
//     onelibrary /Volumes/MYUSB [passphrase]
//
// The first argument is the root of the device -- the directory holding
// `PIONEER` -- or the `exportLibrary.db` file itself.  The second is the
// passphrase, which defaults to the one rekordbox uses.

#include <iostream>
#include <string>

#include <djinterop/djinterop.hpp>

namespace
{
void print_playlist(const djinterop::playlist& pl, int depth)
{
    const std::string indent(static_cast<size_t>(depth) * 2, ' ');
    std::cout << indent << "- " << pl.name() << " (" << pl.tracks().size()
              << " tracks)\n";

    for (auto&& child : pl.children())
        print_playlist(child, depth + 1);
}

}  // anonymous namespace

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "usage: onelibrary <device> [passphrase]\n";
        return 2;
    }

    const std::string device = argv[1];
    const std::string passphrase =
        argc > 2 ? argv[2] : djinterop::onelibrary::default_passphrase;

    if (!djinterop::onelibrary::database_exists(device))
    {
        std::cerr << "No OneLibrary database found in " << device << "\n";
        return 1;
    }

    try
    {
        auto db = djinterop::onelibrary::load_database(device, passphrase);

        std::cout << "Format:  " << db.version_name() << "\n"
                  << "Library: " << db.uuid() << "\n\n";

        std::cout << "Tracks\n------\n";
        for (auto&& track : db.tracks())
        {
            std::cout << track.id() << ". "
                      << track.title().value_or("(untitled)") << " - "
                      << track.artist().value_or("(unknown artist)");

            if (const auto bpm = track.bpm())
                std::cout << " [" << *bpm << " BPM]";

            if (const auto key = track.key())
                std::cout << " [" << *key << "]";

            std::cout << "\n    " << track.relative_path() << "\n";
        }

        std::cout << "\nPlaylists\n---------\n";
        for (auto&& pl : db.root_playlists())
            print_playlist(pl, 0);
    }
    catch (const djinterop::unsupported_database& e)
    {
        // The most likely cause is a passphrase that does not open the
        // database, which happens if rekordbox has changed it.
        std::cerr << "Could not read the library: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
