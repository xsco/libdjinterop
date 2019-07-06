#include <djinterop/database.hpp>
#include <djinterop/impl/track_impl.hpp>

namespace djinterop
{
track_impl::track_impl(int64_t id) noexcept : id_{id} {}

track_impl::~track_impl() noexcept = default;

int64_t track_impl::id() noexcept
{
    return id_;
}

}  // namespace djinterop
