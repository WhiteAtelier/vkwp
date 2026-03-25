#ifndef VKWP_VULKAN_VULKAN_ERROR_HPP
#define VKWP_VULKAN_VULKAN_ERROR_HPP

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <string>
#include <string_view>

namespace vkwp {

///
/// @brief Vulkan から報告されるエラーに対する例外.
///
/// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkResult.html
///
class VulkanError : public std::runtime_error
{
public:
    // ========================================================================================= //
    /// @name Constructor / Destructor
    /// @{
    // ========================================================================================= //

    ///
    /// @brief コンストラクタ.
    ///
    /// @param result
    ///     Vulkan からのエラーリザルトコード.
    ///
    /// @param message
    ///     何をしようとしてエラーになったかなど, 詳細を表す文字列.
    ///
    [[nodiscard]]
    explicit VulkanError(const VkResult result, const std::string_view message) noexcept;

    ///
    /// @brief デストラクタ.
    ///
    ~VulkanError() noexcept = default;

    /// @}
    // ========================================================================================= //
    /// @name Getter
    /// @{
    // ========================================================================================= //

    ///
    /// @brief エラーの内容を VkResult で取得します.
    ///
    VkResult
    result() const noexcept;

    /// @}
    // ========================================================================================= //
    /// @name Utilities
    /// @{
    // ========================================================================================= //

    ///
    /// @brief VkResult の値を検査し, VK_SUCCESS 以外だった場合はメッセージをつけて例外を送出します.
    ///
    /// VK_SUCCESS だった場合は, 何もしません.
    ///
    /// @param result
    ///     Vulkan からのエラーリザルトコード.
    ///
    /// @param message
    ///     何をしようとしてエラーになったかなど, 詳細を表す文字列.
    ///
    /// @throw VulkanError
    ///     result が VK_SUCCESS 以外だった場合に送出される.
    ///
    static void
    check(const VkResult result, const std::string_view message);

    /// @}

private:
    ///
    /// @brief VkResult, message を統合してエラーメッセージを作成します.
    ///
    static std::string
    _toMessage(const VkResult result, const std::string_view message) noexcept;

    int result_;
};

}  // namespace vkwp

#endif
