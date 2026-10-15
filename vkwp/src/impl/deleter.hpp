// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#ifndef VKWP_IMPL_DELETER_HPP
#define VKWP_IMPL_DELETER_HPP

#include <tuple>

namespace vkwp::impl {

template <typename T, typename DFn, typename... ParentTs>
extern void
_delete(T, DFn, const std::tuple<ParentTs...> &) noexcept;

}  // namespace vkwp::impl

#endif
