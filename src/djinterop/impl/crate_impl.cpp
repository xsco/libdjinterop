#include <djinterop/database.hpp>
#include <djinterop/impl/crate_impl.hpp>

namespace djinterop
{
crate_impl::crate_impl(int64_t id) noexcept : id_{id} {}

crate_impl::~crate_impl() noexcept = default;

int64_t crate_impl::id() noexcept
{
    return id_;
}

}  // namespace djinterop
