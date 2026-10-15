// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#ifndef VKWP_UTILS_HPP
#define VKWP_UTILS_HPP

#include <type_traits>

namespace vkwp::utils {

template <typename T>
requires std::is_integral_v<T> && std::is_unsigned_v<T>
constexpr T
align(const T value, const T alignment)
{
    if (alignment != 0)
    {
        return static_cast<T>((value + alignment - 1) / alignment) * alignment;
    }
    else
    {
        return value;
    }
}

static_assert(align<unsigned int>(256, 128) == 256);
static_assert(align<unsigned int>(150, 128) == 256);
static_assert(align<unsigned int>(33, 2) == 34);
static_assert(align<unsigned int>(33, 1) == 33);

}  // namespace vkwp::utils

#endif
