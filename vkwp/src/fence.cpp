#include "vkwp/fence.hpp"

#include "vkwp/vulkan_error.hpp"

vkwp::Fence::Fence() = default;

vkwp::Fence::Fence(const std::string & label)
    : Resource{ label }
{}

bool
vkwp::Fence::wait(const std::uint64_t timeout) const
{
    const auto result = vkWaitForFences(this->_getParent(), 1, this->ptr(), VK_TRUE, timeout);
    if (result == VK_TIMEOUT)
    {
        return false;
    }
    VulkanError::check(result, "Failed to vkWaitForFences().");
    return true;
}

void
vkwp::Fence::resetSignal() const
{
    VulkanError::check(  //
        vkResetFences(this->_getParent(), 1, this->ptr()),
        "Failed to vkResetFences().");
}
