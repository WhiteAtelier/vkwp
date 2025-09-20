#ifndef VKWP_PIPELINE_CACHE_HPP
#define VKWP_PIPELINE_CACHE_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

#include <vector>

namespace vkwp {

///
/// @brief VkPipeline wrapper.
///
class PipelineCache final : public Resource<VkPipelineCache, VkDevice>
{
public:
    using Resource::Resource;

    struct OptionalCreateArgs
    {
        const void *               p_next            = nullptr;
        VkPipelineCacheCreateFlags flags             = 0;
        std::size_t                initial_data_size = 0;
        const void *               initial_data      = nullptr;
        const DebugUtilsResource * debug_utils       = nullptr;
    };

    static const OptionalCreateArgs default_opt_args;

    static PipelineCache
    create(const std::string & label, const Device & device, const OptionalCreateArgs & opt_args = default_opt_args);
};

}  // namespace vkwp

#endif
