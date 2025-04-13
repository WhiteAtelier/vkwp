#ifndef VKWP_DEBUG_REPORT_CALLBACK_RESOURCE_HPP
#define VKWP_DEBUG_REPORT_CALLBACK_RESOURCE_HPP

#ifdef VKWP_DEBUG
#    define VKWP_DEBUG_REPORT_CALLBACK_RESOURCE_ENABLED 1
#else
#    define VKWP_DEBUG_REPORT_CALLBACK_RESOURCE_ENABLED 0
#endif

#if VKWP_DEBUG_REPORT_CALLBACK_RESOURCE_ENABLED
#    include <vulkan/vulkan.h>

namespace vkwp {

///
/// @brief Vulkan Debug Report.
///
/// @warning
///     THIS CLASS IS AVAILABLE UNDER DEBUG BUILD.
///
/// クラス自体を DISABLED 時に定義しない理由は,
/// 事前の Debug Layer の追加など, 処理の場合分けがこのクラスでは完結しないため.
/// このクラスでは定義される/されないという場合分けとすることで,
/// 使用時に場合分けが別途必要になり, Debug Layer などの場合分けと一貫して行うことができる.
///
/// DEBUG ビルド時にこのファイルは実態を持つが,
/// DEBUG ビルド時に Debug Layer を使わない選択もユーザ側は取れる.
/// 最低限 DEBUG ビルドの時のみ有効という前提で, 実際に Debug Layer を使うかどうかは
/// ユーザの選択に任せ, その選択に従って適宜このクラスを利用すること.
///
class DebugReportCallbackResource
{
public:
    DebugReportCallbackResource();
    ~DebugReportCallbackResource() noexcept;

    ///
    /// @brief Instance 生成後に呼び出すべき初期化関数.
    ///
    /// @param instance
    ///     初期化済み Vulkan Instance.
    ///
    void
    initialize(const VkInstance instance);

private:
    static VkBool32 VKAPI_CALL
    _debugReportCallback(VkDebugReportFlagsEXT      flags,
                         VkDebugReportObjectTypeEXT object_type,
                         uint64_t                   object,
                         size_t                     location,
                         int32_t                    message_code,
                         const char *               layer_prefix,
                         const char *               message,
                         void *                     userData);

    VkInstance                          instance_                            = nullptr;
    PFN_vkCreateDebugReportCallbackEXT  pfn_vkCreateDebugReportCallbackEXT_  = nullptr;
    PFN_vkDebugReportMessageEXT         pfn_vkDebugReportMessageEXT_         = nullptr;
    PFN_vkDestroyDebugReportCallbackEXT pfn_vkDestroyDebugReportCallbackEXT_ = nullptr;
    VkDebugReportCallbackEXT            pfn_debugReport_                     = nullptr;
};

}  // namespace vkwp

#endif
#endif
