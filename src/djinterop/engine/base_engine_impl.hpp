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

#pragma once

#include <memory>

namespace djinterop::engine
{
template <typename TThis, typename TEngineLibrary>
class base_engine_impl
{
public:
    explicit base_engine_impl(std::shared_ptr<TEngineLibrary> library) :
        library_{std::move(library)}
    {
    }

    virtual ~base_engine_impl() = default;

protected:
    const TThis* context_cast_maybe(const auto* base_maybe)
    {
        return context_cast_maybe<TThis>(base_maybe);
    }

    const TThis* context_cast_maybe(const auto& base)
    {
        return context_cast_maybe<TThis>(base);
    }

    template<typename TDerived>
    const TDerived* context_cast_maybe(const auto* base_maybe)
    {
        return base_maybe != nullptr ? context_cast_maybe<TDerived>(*base_maybe)
                                     : nullptr;
    }

    template<typename TDerived>
    const TDerived* context_cast_maybe(const auto& base)
    {
        const auto* derived = dynamic_cast<const TDerived*>(&base);

        // TODO(mr-smidge): Need to obtain UUID from library for proper
        //  contextual cast, and compare it against downcasted impl.
        return derived;
    }

    const TThis& context_cast(const auto& base)
    {
        return context_cast<TThis>(base);
    }

    template<typename TDerived>
    const TDerived& context_cast(const auto& base)
    {
        const auto* derived_maybe = context_cast_maybe<TDerived>(base);
        if (derived_maybe == nullptr)
        {
            //TODO(mr-smidge): Use a dedicated exception type.
            throw std::invalid_argument(
                "Supplied argument does not belong to this Engine database");
        }

        return *derived_maybe;
    }

    std::shared_ptr<TEngineLibrary> library_;
};

}  // namespace djinterop::engine
