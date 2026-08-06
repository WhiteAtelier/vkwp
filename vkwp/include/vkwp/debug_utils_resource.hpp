#ifndef VKWP_DEBUG_UTILS_RESOURCE_HPP
#define VKWP_DEBUG_UTILS_RESOURCE_HPP

#include "fwd.hpp"

#include "vulkan.hpp"

#include <memory>
#include <string_view>

namespace vkwp {

///
/// @brief Vulkan Debug Utils.
///
class DebugUtilsResource
{
public:
    DebugUtilsResource();
    DebugUtilsResource(const DebugUtilsResource &);
    DebugUtilsResource(DebugUtilsResource &&) noexcept;
    ~DebugUtilsResource() noexcept;

    DebugUtilsResource &
    operator=(const DebugUtilsResource &) &;

    DebugUtilsResource &
    operator=(DebugUtilsResource &&) & noexcept;

    ///
    /// @brief Instance 生成後に呼び出すべき初期化関数.
    ///
    /// Debug utils を使用しない場合は呼び出さない.
    ///
    /// @param instance
    ///     初期化済み Vulkan Instance.
    ///
    void
    initialize(const VkInstance instance, const VulkanFunctions * functions = nullptr);

    void
    registDebugUtilsObjectName(const VkDevice &       device,
                               const VkObjectType     object_type,
                               const void * const     ptr,
                               const std::string_view name) const;

private:
    class Impl_;
    std::shared_ptr<Impl_> impl_;
};

}  // namespace vkwp

#endif
