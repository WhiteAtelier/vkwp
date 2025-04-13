#include "vkwp/descriptor_set.hpp"

#include "impl/assert.hpp"
#include "vkwp/vulkan_error.hpp"

vkwp::DescriptorSet::DescriptorSet() = default;

vkwp::DescriptorSet::DescriptorSet(const std::string & label)
    : Resource{ label }
{}
