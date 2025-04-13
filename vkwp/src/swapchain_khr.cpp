#include "vkwp/swapchain_khr.hpp"

#include "impl/assert.hpp"
#include "vkwp/vulkan_error.hpp"

vkwp::SwapchainKHR::SwapchainKHR() = default;

vkwp::SwapchainKHR::SwapchainKHR(const std::string & label)
    : Resource{ label }
{}

#if defined(VK_EXT_hdr_metadata) && VK_EXT_hdr_metadata
void
vkwp::SwapchainKHR::setHdrMetadata(const VkHdrMetadataEXT & metadata)
{
#    define GET_INSTANCE_PROC_ADDR(FuncName) \
        reinterpret_cast<PFN_##FuncName>(vkGetDeviceProcAddr(this->_getParent(), #FuncName))
    VKWP_ASSERTION(GET_INSTANCE_PROC_ADDR(vkSetHdrMetadataEXT) != nullptr);
    GET_INSTANCE_PROC_ADDR(vkSetHdrMetadataEXT)(this->_getParent(), 1, this->ptr(), &metadata);
#    undef GET_INSTANCE_PROC_ADDR
}
#endif

std::vector<VkImage>
vkwp::SwapchainKHR::getSwapchainImages() const
{
    std::vector<VkImage> ret;
    std::uint32_t        count = 0;
    VulkanError::check(  //
        vkGetSwapchainImagesKHR(this->_getParent(), this->_ref(), &count, nullptr),
        "Failed to vkGetSwapchainImagesKHR(). (array count)");
    ret.resize(count);
    VulkanError::check(  //
        vkGetSwapchainImagesKHR(this->_getParent(), this->_ref(), &count, ret.data()),
        "Failed to vkGetSwapchainImagesKHR(). (array data)");
    return ret;
}
