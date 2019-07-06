#pragma once

#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/utility/string_view.hpp>

#include <djinterop/crate.hpp>
#include <djinterop/track.hpp>

namespace djinterop
{
class database_impl
{
public:
    virtual ~database_impl();

    virtual boost::optional<crate> crate_by_id(int64_t id) = 0;
    virtual std::vector<crate> crates() = 0;
    virtual std::vector<crate> crates_by_name(boost::string_view name) = 0;
    virtual crate create_crate(boost::string_view name) = 0;
    virtual track create_track(boost::string_view relative_path) = 0;
    virtual std::string directory() = 0;
    virtual bool is_supported() = 0;
    virtual void verify() = 0;
    virtual std::string music_db_path() = 0;
    virtual std::string perfdata_db_path() = 0;
    virtual void remove_crate(crate cr) = 0;
    virtual void remove_track(track tr) = 0;
    virtual std::vector<crate> root_crates() = 0;
    virtual boost::optional<track> track_by_id(int64_t id) = 0;
    virtual std::vector<track> tracks() = 0;
    virtual std::vector<track> tracks_by_relative_path(
        boost::string_view relative_path) = 0;
    virtual std::string uuid() = 0;
    virtual semantic_version version() = 0;
};

}  // namespace djinterop
