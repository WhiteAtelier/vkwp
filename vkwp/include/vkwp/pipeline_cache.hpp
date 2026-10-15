// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#ifndef VKWP_PIPELINE_CACHE_HPP
#define VKWP_PIPELINE_CACHE_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <string>
#include <vector>

namespace vkwp {

///
/// @brief VkPipeline wrapper.
///
class PipelineCache final : public Resource<VkPipelineCache, PFN_vkDestroyPipelineCache, VkDevice>
{
public:
    using Resource<VkPipelineCache, PFN_vkDestroyPipelineCache, VkDevice>::Resource;

    [[nodiscard]]
    static PipelineCache
    importExternal(std::string label, const Device & device, const VkPipelineCache external);

    struct OptionalCreateArgs
    {
        const void *               p_next            = nullptr;
        VkPipelineCacheCreateFlags flags             = 0;
        std::size_t                initial_data_size = 0;
        const void *               initial_data      = nullptr;
        const DebugUtilsResource * debug_utils       = nullptr;
        const VulkanFunctions *    functions         = nullptr;
    };

    static PipelineCache
    create(std::string label, const Device & device);

    static PipelineCache
    create(std::string label, const Device & device, const OptionalCreateArgs & opt_args);
};

}  // namespace vkwp

#endif
