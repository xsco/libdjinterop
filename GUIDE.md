Guide to libdjinterop
=====================

This documentation provides an introductory guide to the public API of
`libdjinterop`, and provides information on how to get started using it.

High-level vs. Low-level APIs
-----------------------------

The library offers so-called "high-level" and "low-level" APIs, which serve
different purposes.

Low-level APIs
--------------

The low-level APIs provides a very thin wrapper over a specific DJ database
library format.  A low-level API exposes all the peculiarities, details, and
expressivity of that format, and does not offer any abstractions beyond
presenting a C++ library interface.

There are multiple low-level APIs, one for each type of DJ library format.
There can also be multiple low-level APIs to cover occasions where a library
undergoes a significant change to its schema (e.g. Engine 1.x and Engine 2.x
are distinct low-level APIs).

Sometimes, a low-level API (or collection of them) offer utility functions that
help the end user translate from common concepts into encodings or types that
are specific to the low-level library format.  An example could be calculating
the preferred size of a waveform for a given format.

The current low-level APIs offered are listed below:

| Low-level API | Include Path                                        |
|---------------|-----------------------------------------------------|
| Engine 2.x    | `#include <djinterop/engine/v2/engine_library.hpp>` |
| Engine 3.x    | `#include <djinterop/engine/v3/engine_library.hpp>` |

Note that there is no low-level API for Engine 1.x at present.  This is because
when the library was first being written, Engine 1.x support was introduced
directly into the high-level API without a low-level companion.  The low-level
API was developed with the introduction of Engine 2.x, and because the 1.x
library was effectively now deprecated, the effort was not undertaken to
backport the idea to 1.x.  If a low-level API for Engine 1.x is important to
you, please consider contributing!

High-level API
--------------

The library offers a high-level API as well, with a very different purpose to
that of the low-level APIs.  Whereas low-level APIs simply expose an underlying
DJ library format in the form of a C++ API, the high-level API aims to:

* Abstract over all format-specific aspects.
* Offer a uniform interface, regardless of the underlying DJ library format.
* Offer meaningful DJ library concepts, in the form of lightweight handles to
  objects in the underlying DJ library, that can be read or updated.

The primary concepts in the high-level API are:

* _Database_, representing the overall DJ library.
* _Track_, representing a lightweight handle to a track in the library.
* _Track snapshot_, representing a collection of offline data read from a track
  in the library.
* _Crate_, representing an unordered list of related tracks.
* _Playlist_, representing an ordered list of related tracks.

Part of the challenge in developing `libdjinterop` is to ensure that the
high-level API appropriately abstracts over all the different ways that the
above concepts might be implemented in a real DJ library format.

In reality, a DJ library that exists on disk is always of a specific format.
As such, in order to create a new library or load an existing library with the
intention of operating on it using the high-level API, it is always necessary
to start with format-specific functions to do so:

| Library Type | Include Path                                     |
|--------------|--------------------------------------------------|
| Engine       | `#include <djinterop/engine/engine.hpp>`         |
| OneLibrary   | `#include <djinterop/onelibrary/onelibrary.hpp>` |

OneLibrary
----------

The AlphaTheta OneLibrary format, also documented as Device Library Plus, is
the successor to the DeviceSQL `export.pdb` library that rekordbox wrote to USB
media.  A database is loaded by way of `onelibrary::load_database()`, given
either the root directory of a device or the database file itself.  A number of
aspects of the format are worth noting:

* Support is currently read-only, and everything that would change a database
  throws `djinterop::unsupported_operation`.
* Beat grids, waveforms, hot cues and loops are not held in the database.
  rekordbox leaves them in the ANLZ files that `content.analysisDataFilePath`
  points at, and does not populate the `cue` table on export.  Those accessors
  therefore return nothing rather than throwing.  A caller that reads ANLZ
  files itself can load the device as an `onelibrary::library`, whose
  `analysis_path()` gives the path recorded for a track, relative to the root
  of the device; `library::db()` then gives the same database that
  `load_database()` would have.
* `onelibrary::library` also reaches the two other things a device carries
  that the format-agnostic interface has nowhere to put: `key_name()` gives
  the musical key in the notation rekordbox wrote, which may be Camelot and
  which `track::key()` cannot represent, and `color_id()` gives the colour the
  DJ marked a track with, numbered as `export.pdb` numbers them.
* The format has a single tree that serves as both playlists and crates, so
  `playlists_and_crates_are_distinct` is false and the two views show the same
  rows.
* A device is read by decrypting it into memory.  rekordbox writes the library
  in write-ahead-logged mode, and the log has to be folded in before SQLite
  sees the file, so reading one needs SQLite 3.36 or newer, built without
  `SQLITE_OMIT_DESERIALIZE`.


Stable API/ABI
--------------

The library does not yet commit to offer either a stable API nor ABI.  This
situation may change in the future as the library matures.
