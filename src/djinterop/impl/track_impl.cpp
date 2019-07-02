#include <djinterop/database.hpp>
#include <djinterop/impl/track_impl.hpp>

namespace djinterop
{
int64_t track_impl::id()
{
    return id_;
}

track_impl::track_impl(int64_t id) : id_{id} {}

}  // namespace djinterop
