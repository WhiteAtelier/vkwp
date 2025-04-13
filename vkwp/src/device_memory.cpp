#include "vkwp/device_memory.hpp"

#include "vkwp/vulkan_error.hpp"

#include <cstring>

vkwp::DeviceMemory::DeviceMemory() = default;

vkwp::DeviceMemory::DeviceMemory(const std::string & label)
    : Resource{ label }
{}

namespace {

class Scope final
{
public:
    Scope(const VkDevice &       device,
          const VkDeviceMemory & device_memory,
          const std::size_t      offset,
          const std::size_t      size)
        : device_{ device }
        , memory_{ device_memory }
        , ptr_{ nullptr }
    {
        vkwp::VulkanError::check(  //
            vkMapMemory(device, device_memory, offset, size, 0, &this->ptr_),
            "Failed to vkMapMemory().");
    }

    ~Scope()
    {
        if (this->ptr_ != nullptr)
        {
            vkUnmapMemory(this->device_, this->memory_);
        }
    }

    void *
    ptr() const noexcept
    {
        return this->ptr_;
    }

private:
    VkDevice       device_;
    VkDeviceMemory memory_;
    void *         ptr_;
};

}  // namespace

void
vkwp::DeviceMemory::write(const void * const src, const std::size_t size, const std::size_t offset) const
{
    Scope scope{ this->_getParent(), this->_ref(), offset, size };
    std::memcpy(scope.ptr(), src, size);
}

void
vkwp::DeviceMemory::read(void * const dst, const std::size_t size, const std::size_t offset) const
{
    Scope scope{ this->_getParent(), this->_ref(), offset, size };
    std::memcpy(dst, scope.ptr(), size);
}
