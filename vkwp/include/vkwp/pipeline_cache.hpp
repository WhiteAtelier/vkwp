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
    static PipelineCache
    create(const std::string & label, const Device & device);
};

}  // namespace vkwp

#endif
