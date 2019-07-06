#pragma once

#include <string>
#include <vector>

#include <boost/utility/string_view.hpp>

#include <djinterop/crate.hpp>
#include <djinterop/impl/database_impl.hpp>
#include <djinterop/track.hpp>

namespace djinterop
{
class database;
class track;

class crate_impl
{
public:
    crate_impl(int64_t id) noexcept;
    virtual ~crate_impl() noexcept;

    int64_t id() noexcept;

    virtual void add_track(track tr) = 0;
    virtual std::vector<crate> children() = 0;
    virtual void clear_tracks() = 0;
    virtual database db() = 0;
    virtual std::vector<crate> descendants() = 0;
    virtual bool is_valid() = 0;
    virtual std::string name() = 0;
    virtual boost::optional<crate> parent() = 0;
    virtual void remove_track(track tr) = 0;
    virtual void set_name(boost::string_view name) = 0;
    virtual void set_parent(boost::optional<crate> parent) = 0;
    virtual std::vector<track> tracks() = 0;

private:
    int64_t id_;
};

}  // namespace djinterop
