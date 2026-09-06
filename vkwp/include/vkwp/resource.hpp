#ifndef VKWP_VKM_RESOURCE_HPP
#define VKWP_VKM_RESOURCE_HPP

#include <cstddef>
#include <string>
#include <tuple>
#include <type_traits>

namespace vkwp {

static_assert(std::is_copy_assignable_v<std::string>);
static_assert(std::is_nothrow_move_constructible_v<std::string>, "std::string must be nothrow move constructible.");
static_assert(std::is_nothrow_move_assignable_v<std::string>, "std::string must be nothrow move assignable.");

template <typename T>
concept vulkan_raw_resource = std::is_pointer_v<T>;

template <::vkwp::vulkan_raw_resource T, typename DestroyFn, typename... ParentTs>
class Resource
{
public:
    explicit Resource();

    Resource(const Resource &) = delete;

    [[nodiscard]]
    Resource(Resource &&) noexcept;

    virtual ~Resource() noexcept;

    Resource &
    operator=(const Resource &)
        = delete;

    Resource &
    operator=(Resource &&) & noexcept;

    [[nodiscard]]
    explicit
    operator bool() const & noexcept;

    [[nodiscard]]
    bool
    operator!() const & noexcept;

    [[nodiscard]]
    const std::string &
    getLabel() const & noexcept;

    [[nodiscard]]
    bool
    isExternal() const & noexcept;

    void
    reset() & noexcept;

    [[nodiscard]] operator T() const & noexcept;

    [[nodiscard]]
    const T *
    ptr() const & noexcept;

    // clang-format off
    Resource & operator=(Resource &&) && noexcept = delete; ///< ref-qualifier: rvalue deleted.
    explicit operator bool() && noexcept          = delete; ///< ref-qualifier: rvalue deleted.
    bool operator!()  && noexcept                 = delete; ///< ref-qualifier: rvalue deleted.
    operator T() && noexcept                      = delete; ///< ref-qualifier: rvalue deleted.
    const T * ptr() && noexcept                   = delete; ///< ref-qualifier: rvalue deleted.
    const std::string & getLabel() && noexcept    = delete; ///< ref-qualifier: rvalue deleted.
    bool isExternal() && noexcept                 = delete; ///< ref-qualifier: rvalue deleted.
    // clang-format on

protected:
    using BaseType = Resource<T, DestroyFn, ParentTs...>;

    Resource(std::string && label);

    [[nodiscard]]
    explicit Resource(std::string && label, const T & external, ParentTs... parents);

    T *
    _reset(DestroyFn fn, ParentTs... parents) & noexcept;

private:
    std::string label_;
    T           instance_;
    DestroyFn   destroy_fn_;
    bool        external_;

protected:
    std::tuple<ParentTs...> parents_;
};

}  // namespace vkwp

#endif
