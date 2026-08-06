#ifndef VKWP_DEBUG_REPORT_CALLBACK_RESOURCE_HPP
#define VKWP_DEBUG_REPORT_CALLBACK_RESOURCE_HPP

#include "fwd.hpp"

#include "vulkan.hpp"

#include <memory>

namespace vkwp {

///
/// @brief Vulkan Debug Report.
///
class DebugReportCallbackResource
{
public:
    DebugReportCallbackResource();
    ~DebugReportCallbackResource() noexcept;

    ///
    /// @brief Instance 生成後に呼び出すべき初期化関数.
    ///
    /// Debug report を使用しない場合は呼び出さない.
    ///
    /// @param instance
    ///     初期化済み Vulkan Instance.
    ///
    void
    initialize(const VkInstance instance, const VulkanFunctions * functions = nullptr);

private:
    class Impl_;
    std::unique_ptr<Impl_> impl_;
};

}  // namespace vkwp

#endif
