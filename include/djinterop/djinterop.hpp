#pragma once

#if __cplusplus < 201103L && _MSVC_LANG < 201103L
#error This library needs at least a C++11 compliant compiler
#endif

#include <djinterop/album_art.hpp>
#include <djinterop/crate.hpp>
#include <djinterop/database.hpp>
#include <djinterop/enginelibrary.hpp>
#include <djinterop/exceptions.hpp>
#include <djinterop/musical_key.hpp>
#include <djinterop/pad_color.hpp>
#include <djinterop/performance_data.hpp>
#include <djinterop/semantic_version.hpp>
#include <djinterop/track.hpp>