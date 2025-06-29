#include "vkwp/descriptor_pool.hpp"

#include "impl/assert.hpp"
#include "vkwp/descriptor_set.hpp"
#include "vkwp/vulkan_error.hpp"

vkwp::DescriptorPool::DescriptorPool() = default;

vkwp::DescriptorPool::DescriptorPool(const std::string & label)
    : Resource{ label }
{}

void
vkwp::DescriptorPool::setDebugUtilsResource(const DebugUtilsResource & debug_utils_resource)
{
    this->debug_utils_resource_ = debug_utils_resource;
}

std::vector<vkwp::DescriptorSet>
vkwp::DescriptorPool::allocateDescriptorSets(const std::string &                        label,
                                             const std::vector<VkDescriptorSetLayout> & set_layouts,
                                             const void * const                         p_next) const
{
    std::vector<DescriptorSet> ret;
    ret.reserve(set_layouts.size());
    for (std::size_t i = 0; i < set_layouts.size(); ++i)
    {
        ret.emplace_back(label + std::to_string(i));
    }

    std::vector<VkDescriptorSet>      dsets(set_layouts.size(), nullptr);
    const VkDescriptorSetAllocateInfo ai{
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = p_next,
        .descriptorPool     = this->_ref(),
        .descriptorSetCount = static_cast<std::uint32_t>(set_layouts.size()),
        .pSetLayouts        = set_layouts.data(),
    };
    VulkanError::check(  //
        vkAllocateDescriptorSets(this->_getParent(), &ai, dsets.data()),
        "Failed to vkAllocateDescriptorSets().");

    for (std::size_t i = 0; i < dsets.size(); ++i)
    {
        *ret[i].reset(this->_getParent(), this->_ref()) = dsets[i];
    }

    return ret;
}

vkwp::DescriptorSet
vkwp::DescriptorPool::allocateDescriptorSet(const std::string &           label,
                                            const VkDescriptorSetLayout & set_layouts,
                                            const void * const            p_next) const
{
    DescriptorSet                     ret{ label };
    const VkDescriptorSetAllocateInfo ai{
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = p_next,
        .descriptorPool     = this->_ref(),
        .descriptorSetCount = 1,
        .pSetLayouts        = &set_layouts,
    };
    VulkanError::check(  //
        vkAllocateDescriptorSets(this->_getParent(), &ai, ret.reset(this->_getParent(), this->_ref())),
        "Failed to vkAllocateDescriptorSets().");
    return ret;
}
