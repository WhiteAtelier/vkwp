// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#ifndef VKWP_TEST_UTILS_HPP
#define VKWP_TEST_UTILS_HPP

#include <vkwp/vulkan.hpp>

#include <cstdint>
#include <type_traits>

namespace vkwp::test {

template <typename T>
T
fakeHandle(const std::uintptr_t value)
{
    if constexpr (std::is_pointer_v<T>)
    {
        return reinterpret_cast<T>(value);
    }
    else
    {
        return static_cast<T>(value);
    }
}

inline const void *
fakePointer(const std::uintptr_t value)
{
    return reinterpret_cast<const void *>(value);
}

}  // namespace vkwp::test

#endif
