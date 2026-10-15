// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#ifndef VKWP_INSTANCE_HPP
#define VKWP_INSTANCE_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace vkwp {

///
/// @brief VkInstance wrapper.
///
class Instance final : public Resource<VkInstance, PFN_vkDestroyInstance>
{
public:
    using Resource<VkInstance, PFN_vkDestroyInstance>::Resource;

    [[nodiscard]]
    static Instance
    importExternal(std::string label, const VkInstance external);

    struct OptionalCreateArgs
    {
        const void *            p_next    = nullptr;
        const VulkanFunctions * functions = nullptr;
    };

    [[nodiscard]]
    static Instance
    create(std::string                      label,
           const std::string &              application_name,
           const std::uint32_t              application_version,
           const std::string &              engine_name,
           const std::uint32_t              engine_version,
           const std::uint32_t              api_version,
           const std::vector<std::string> & enabled_layer_names,
           const std::vector<std::string> & enabled_extension_names);

    [[nodiscard]]
    static Instance
    create(std::string                      label,
           const std::string &              application_name,
           const std::uint32_t              application_version,
           const std::string &              engine_name,
           const std::uint32_t              engine_version,
           const std::uint32_t              api_version,
           const std::vector<std::string> & enabled_layer_names,
           const std::vector<std::string> & enabled_extension_names,
           const OptionalCreateArgs &       opt_args);

    [[nodiscard]]
    static std::vector<VkExtensionProperties>
    enumerateInstanceExtensionProperties(const VulkanFunctions * functions = nullptr);
};

}  // namespace vkwp

#endif
