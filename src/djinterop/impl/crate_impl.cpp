#include <djinterop/database.hpp>
#include <djinterop/impl/crate_impl.hpp>

namespace djinterop
{
int64_t crate_impl::id()
{
    return id_;
}

crate_impl::crate_impl(int64_t id) : id_{id} {}

}  // namespace djinterop
