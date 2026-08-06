#ifndef VKWP_DEBUG_REPORT_CALLBACK_RESOURCE_HPP
#define VKWP_DEBUG_REPORT_CALLBACK_RESOURCE_HPP

#include "fwd.hpp"

#include "vulkan.hpp"

#include <memory>

namespace vkwp {

struct DebugReportCallback
{
public:
    DebugReportCallback();
    virtual ~DebugReportCallback() noexcept;

    DebugReportCallback(const DebugReportCallback &) = delete;
    DebugReportCallback(DebugReportCallback &&)      = delete;
    DebugReportCallback &
    operator=(const DebugReportCallback &)
        = delete;
    DebugReportCallback &
    operator=(DebugReportCallback &&)
        = delete;

    virtual void
    operator()(const VkDebugReportFlagsEXT      flags,
               const VkDebugReportObjectTypeEXT object_type,
               const std::uint64_t              object,
               const std::size_t                location,
               const std::int32_t               message_code,
               const char *                     layer_prefix,
               const char *                     message)
        = 0;
};

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
    initialize(const VkInstance                     instance,
               std::shared_ptr<DebugReportCallback> callback,
               const VulkanFunctions *              functions = nullptr);

private:
    class Impl_;
    std::unique_ptr<Impl_> impl_;
};

}  // namespace vkwp

#endif
