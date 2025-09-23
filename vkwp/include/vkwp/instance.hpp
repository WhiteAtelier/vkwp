#ifndef VKWP_INSTANCE_HPP
#define VKWP_INSTANCE_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

#include <vector>

namespace vkwp {

///
/// @brief VkInstance wrapper.
///
class Instance final : public Resource<VkInstance>
{
public:
    using Resource<VkInstance>::Resource;

    struct OptionalCreateArgs
    {
        const void * p_next = nullptr;
    };

    static const OptionalCreateArgs default_opt_args;

    [[nodiscard]]
    static Instance
    create(const std::string &            label,
           const std::string &            application_name,
           const std::uint32_t            application_version,
           const std::string &            engine_name,
           const std::uint32_t            engine_version,
           const std::uint32_t            api_version,
           const std::vector<std::string> enabled_layer_names,
           const std::vector<std::string> enabled_extension_names,
           const OptionalCreateArgs &     opt_args = default_opt_args);

    [[nodiscard]]
    static std::vector<VkExtensionProperties>
    enumerateInstanceExtensionProperties();
};

}  // namespace vkwp

#endif
