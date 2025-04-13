#include "vkwp/descriptor_set_layout.hpp"

#include "impl/assert.hpp"
#include "vkwp/vulkan_error.hpp"

vkwp::DescriptorSetLayout::DescriptorSetLayout() = default;

vkwp::DescriptorSetLayout::DescriptorSetLayout(const std::string & label)
    : Resource{ label }
{}
