// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#ifndef VKWP_SHADER_MODULE_HPP
#define VKWP_SHADER_MODULE_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace vkwp {

///
/// @brief VkShaderModule wrapper.
///
class ShaderModule final : public Resource<VkShaderModule, PFN_vkDestroyShaderModule, VkDevice>
{
public:
    using Resource<VkShaderModule, PFN_vkDestroyShaderModule, VkDevice>::Resource;

    [[nodiscard]]
    static ShaderModule
    importExternal(std::string label, const Device & device, const VkShaderModule external);

    struct OptionalCreateArgs
    {
        const void *               p_next      = nullptr;
        VkShaderModuleCreateFlags  flags       = 0;
        const DebugUtilsResource * debug_utils = nullptr;
        const VulkanFunctions *    functions   = nullptr;
    };

    [[nodiscard]]
    static ShaderModule
    create(std::string                        label,  //
           const Device &                     device,
           const std::vector<std::uint32_t> & code);

    [[nodiscard]]
    static ShaderModule
    create(std::string                        label,
           const Device &                     device,
           const std::vector<std::uint32_t> & code,
           const OptionalCreateArgs &         opt_args);

    [[nodiscard]]
    static ShaderModule
    create(std::string label, const Device & device, const std::uint32_t * const data, const std::size_t data_length);

    [[nodiscard]]
    static ShaderModule
    create(std::string                 label,
           const Device &              device,
           const std::uint32_t * const data,
           const std::size_t           data_length,
           const OptionalCreateArgs &  opt_args);
};

}  // namespace vkwp

#endif
