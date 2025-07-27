#ifndef VKWP_IMPL_DELETER_HPP
#define VKWP_IMPL_DELETER_HPP

#include <tuple>

namespace vkwp::impl {

template <typename T, typename... ParentTs>
extern void
_delete(T, const std::tuple<ParentTs...> &) noexcept;

}  // namespace vkwp::impl

#endif
