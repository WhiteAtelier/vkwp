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
