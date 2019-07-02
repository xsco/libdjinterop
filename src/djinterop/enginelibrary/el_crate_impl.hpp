#pragma once

#include <djinterop/impl/crate_impl.hpp>

namespace djinterop
{
class track;

namespace enginelibrary
{
class el_storage;

class el_crate_impl : public djinterop::crate_impl
{
public:
    el_crate_impl(std::shared_ptr<el_storage> storage, int64_t id);

    void add_track(track tr) override;
    std::vector<crate> children() override;
    void clear_tracks() override;
    database db() override;
    std::vector<crate> descendants() override;
    bool is_valid() override;
    std::string name() override;
    boost::optional<crate> parent() override;
    void remove_track(track tr) override;
    void set_name(boost::string_view name) override;
    void set_parent(boost::optional<crate> parent) override;
    std::vector<track> tracks() override;

private:
    std::shared_ptr<el_storage> storage_;
};

}  // namespace enginelibrary
}  // namespace djinterop
