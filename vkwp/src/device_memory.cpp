#include "vkwp/device_memory.hpp"

#include "vkwp/vulkan_error.hpp"

#include <cstring>

vkwp::DeviceMemory::DeviceMemory() = default;

vkwp::DeviceMemory::DeviceMemory(const std::string & label)
    : Resource{ label }
{}

void
vkwp::DeviceMemory::write(const void * const src, const std::size_t size, const std::size_t offset) const
{
    auto scope = this->map(size, offset);
    std::memcpy(scope.ptr(), src, size);
}

void
vkwp::DeviceMemory::read(void * const dst, const std::size_t size, const std::size_t offset) const
{
    auto scope = this->map(size, offset);
    std::memcpy(dst, scope.ptr(), size);
}

vkwp::DeviceMemory::MappedScope
vkwp::DeviceMemory::map(const std::size_t size, const std::size_t offset) const
{
    return MappedScope{ this->_getParent(), this->_ref(), offset, size };
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// DeviceMemory::MappedScope class implementation
//
///////////////////////////////////////////////////////////////////////////////////////////////////

vkwp::DeviceMemory::MappedScope::MappedScope(const VkDevice &       device,
                                             const VkDeviceMemory & device_memory,
                                             const std::size_t      offset,
                                             const std::size_t      size)
    : device_{ device }
    , memory_{ device_memory }
    , size_{ size }
    , ptr_{ nullptr }
{
    vkwp::VulkanError::check(  //
        vkMapMemory(device, device_memory, offset, size, 0, &this->ptr_),
        "Failed to vkMapMemory().");
}

vkwp::DeviceMemory::MappedScope::MappedScope() noexcept
    : device_{ nullptr }
    , memory_{ nullptr }
    , size_{ 0 }
    , ptr_{ nullptr }
{}

vkwp::DeviceMemory::MappedScope::MappedScope(MappedScope && m) noexcept
    : device_{ m.device_ }
    , memory_{ m.memory_ }
    , size_{ m.size_ }
    , ptr_{ m.ptr_ }
{
    m.device_ = nullptr;
    m.memory_ = nullptr;
    m.size_   = 0;
    m.ptr_    = nullptr;
}

vkwp::DeviceMemory::MappedScope::~MappedScope() noexcept
{
    this->unmap();
}

vkwp::DeviceMemory::MappedScope &
vkwp::DeviceMemory::MappedScope::operator=(MappedScope && rhs) noexcept
{
    if (this != &rhs)
    {
        this->unmap();
        this->device_ = rhs.device_;
        this->memory_ = rhs.memory_;
        this->size_   = rhs.size_;
        this->ptr_    = rhs.ptr_;
        rhs.device_   = nullptr;
        rhs.memory_   = nullptr;
        rhs.size_     = 0;
        rhs.ptr_      = nullptr;
    }
    return *this;
}

vkwp::DeviceMemory::MappedScope::operator bool() const noexcept
{
    return this->ptr_ != nullptr;
}

bool
vkwp::DeviceMemory::MappedScope::operator!() const noexcept
{
    return !static_cast<bool>(*this);
}

bool
vkwp::DeviceMemory::MappedScope::isMapped() const noexcept
{
    return static_cast<bool>(*this);
}

void
vkwp::DeviceMemory::MappedScope::unmap() noexcept
{
    if (this->ptr_ != nullptr)
    {
        vkUnmapMemory(this->device_, this->memory_);
        this->device_ = nullptr;
        this->memory_ = nullptr;
        this->size_   = 0;
        this->ptr_    = nullptr;
    }
}

std::size_t
vkwp::DeviceMemory::MappedScope::size() const noexcept
{
    return this->size_;
}

void *
vkwp::DeviceMemory::MappedScope::ptr() noexcept
{
    return this->ptr_;
}

const void *
vkwp::DeviceMemory::MappedScope::ptr() const noexcept
{
    return this->ptr_;
}
