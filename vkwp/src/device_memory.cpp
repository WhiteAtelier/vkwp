#include "vkwp/device_memory.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"

#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/device.hpp"
#include "vkwp/physical_device.hpp"
#include "vkwp/vulkan_error.hpp"
#include "vkwp/vulkan_functions.hpp"

#include <spdlog/spdlog.h>

#include <cstring>

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete(VkDeviceMemory i, PFN_vkFreeMemory fn, const std::tuple<VkDevice> & args) noexcept
{
    fn(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CommandPool class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::DeviceMemory
vkwp::DeviceMemory::allocate(std::string                  label,
                             const Device &               device,
                             const PhysicalDevice &       physical_device,
                             const VkMemoryRequirements & memory_requirements,
                             const VkMemoryPropertyFlags  memory_property_flags)
{
    return DeviceMemory::allocate(std::move(label),  //
                                  device,
                                  physical_device,
                                  memory_requirements,
                                  memory_property_flags,
                                  {});
}

/* STATIC */
vkwp::DeviceMemory
vkwp::DeviceMemory::allocate(std::string                  label,
                             const Device &               device,
                             const PhysicalDevice &       physical_device,
                             const VkMemoryRequirements & memory_requirements,
                             const VkMemoryPropertyFlags  memory_property_flags,
                             const OptionalAllocateArgs & opt_args)
{
    VKWP_VALIDATE_ARG(device);
    VKWP_VALIDATE_ARG(physical_device);

    DeviceMemory ret{ std::move(label) };
    const auto   memory_type_index
        = physical_device.getMemoryTypeIndex(memory_property_flags, memory_requirements.memoryTypeBits);
    if (!memory_type_index.first)
    {
        SPDLOG_CRITICAL("Available memory is not found. memoryPropertyFlags={}, Required memoryTypeBits={}",
                        memory_property_flags,
                        memory_requirements.memoryTypeBits);
        throw VulkanError{ VK_ERROR_FEATURE_NOT_PRESENT, "Available memory is not found" };
    }

    const VkMemoryAllocateInfo ai{
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = opt_args.p_next,
        .allocationSize  = memory_requirements.size,
        .memoryTypeIndex = memory_type_index.second,
    };
    SPDLOG_DEBUG("DeviceMemory: {} - {}Bytes, {:.03f}MiB",
                 ret.getLabel(),
                 memory_requirements.size,
                 static_cast<double>(memory_requirements.size) / 1024.0 / 1024.0);
    auto * const allocate_fn = (opt_args.functions != nullptr && opt_args.functions->vkAllocateMemory != nullptr)
                                 ? opt_args.functions->vkAllocateMemory
                                 : vkAllocateMemory;
    auto * const free_fn     = (opt_args.functions != nullptr && opt_args.functions->vkFreeMemory != nullptr)
                                 ? opt_args.functions->vkFreeMemory
                                 : vkFreeMemory;

    VulkanError::check(allocate_fn(device, &ai, nullptr, ret._reset(free_fn, device)),  //
                       "Failed to vkAllocateMemory().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_DEVICE_MEMORY,
                                                         static_cast<VkDeviceMemory>(ret),
                                                         ret.getLabel());
    }
    if (opt_args.functions != nullptr)
    {
        ret._vkMapMemory = (opt_args.functions->vkMapMemory != nullptr) ? opt_args.functions->vkMapMemory : vkMapMemory;
        ret._vkUnmapMemory
            = (opt_args.functions->vkUnmapMemory != nullptr) ? opt_args.functions->vkUnmapMemory : vkUnmapMemory;
    }
    else
    {
        ret._vkMapMemory   = vkMapMemory;
        ret._vkUnmapMemory = vkUnmapMemory;
    }
    return ret;
}

vkwp::DeviceMemory
vkwp::DeviceMemory::importExternal(std::string                   label,
                                   const Device &                device,
                                   const VkDeviceMemory          external,
                                   const VulkanFunctions * const functions)
{
    VKWP_VALIDATE_ARG(device);
    VKWP_VALIDATE_ARG(external);

    DeviceMemory ret{ std::move(label), external, static_cast<VkDevice>(device) };
    ret._vkMapMemory
        = (functions != nullptr && functions->vkMapMemory != nullptr) ? functions->vkMapMemory : vkMapMemory;
    ret._vkUnmapMemory
        = (functions != nullptr && functions->vkUnmapMemory != nullptr) ? functions->vkUnmapMemory : vkUnmapMemory;
    return ret;
}

#if VKWP_SUPPORT_WIN32_SURFACE && defined(_WIN32) && _WIN32
/* STATIC */
vkwp::DeviceMemory
vkwp::DeviceMemory::importWin32ExternalMemory(std::string                              label,
                                              const Device &                           device,
                                              const PhysicalDevice &                   physical_device,
                                              const std::size_t                        size,
                                              const VkExternalMemoryHandleTypeFlagBits handle_type,
                                              void * const                             handle,
                                              const wchar_t * const                    name,
                                              const VkImage &                          image,
                                              const VkBuffer &                         buffer)
{
    return DeviceMemory::importWin32ExternalMemory(std::move(label),
                                                   device,
                                                   physical_device,
                                                   size,
                                                   handle_type,
                                                   handle,
                                                   name,
                                                   image,
                                                   buffer,
                                                   {});
}

/* STATIC */
vkwp::DeviceMemory
vkwp::DeviceMemory::importWin32ExternalMemory(std::string                              label,
                                              const Device &                           device,
                                              const PhysicalDevice &                   physical_device,
                                              const std::size_t                        size,
                                              const VkExternalMemoryHandleTypeFlagBits handle_type,
                                              void * const                             handle,
                                              const wchar_t * const                    name,
                                              const VkImage &                          image,
                                              const VkBuffer &                         buffer,
                                              const OptionalAllocateArgs &             opt_args)
{
    VKWP_VALIDATE_ARG(device);
    VKWP_VALIDATE_ARG(physical_device);

    VkMemoryWin32HandlePropertiesKHR memprops{
        .sType          = VK_STRUCTURE_TYPE_MEMORY_WIN32_HANDLE_PROPERTIES_KHR,
        .pNext          = nullptr,
        .memoryTypeBits = 0,
    };
    auto * get_memory_win32_handle_properties_fn
        = (opt_args.functions != nullptr && opt_args.functions->vkGetMemoryWin32HandlePropertiesKHR != nullptr)
            ? opt_args.functions->vkGetMemoryWin32HandlePropertiesKHR
            : nullptr;
    if (get_memory_win32_handle_properties_fn == nullptr)
    {
        auto * const get_device_proc_addr_fn
            = (opt_args.functions != nullptr && opt_args.functions->vkGetDeviceProcAddr != nullptr)
                ? opt_args.functions->vkGetDeviceProcAddr
                : vkGetDeviceProcAddr;
        get_memory_win32_handle_properties_fn = reinterpret_cast<PFN_vkGetMemoryWin32HandlePropertiesKHR>(
            get_device_proc_addr_fn(device, "vkGetMemoryWin32HandlePropertiesKHR"));
    }
    if (get_memory_win32_handle_properties_fn == nullptr)
    {
        SPDLOG_CRITICAL(
            "Failed to get vkGetMemoryWin32HandlePropertiesKHR function. Missing VK_KHR_external_memory_win32 extension?");
        throw std::runtime_error{ "Failed to get vkGetMemoryWin32HandlePropertiesKHR function." };
    }
    VulkanError::check(  //
        get_memory_win32_handle_properties_fn(device, handle_type, handle, &memprops),
        "Failed to vkGetMemoryWin32HandlePropertiesKHR()");

    const VkMemoryDedicatedAllocateInfo memory_dedicated_ai{
        .sType  = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO,
        .pNext  = opt_args.p_next,
        .image  = image,
        .buffer = buffer,
    };
    const VkImportMemoryWin32HandleInfoKHR import_memory_win32_handle_info_khr{
        .sType      = VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_KHR,
        .pNext      = &memory_dedicated_ai,
        .handleType = handle_type,
        .handle     = handle,
        .name       = name,
    };

    return DeviceMemory::allocate(std::move(label),
                                  device,
                                  physical_device,
                                  VkMemoryRequirements{
                                      .size           = size,
                                      .alignment      = 1,  // not used.
                                      .memoryTypeBits = memprops.memoryTypeBits,
                                  },
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                  {
                                      .p_next      = &import_memory_win32_handle_info_khr,
                                      .debug_utils = opt_args.debug_utils,
                                      .functions   = opt_args.functions,
                                  });
}
#endif

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
    VKWP_VALIDATE_THIS;
    return MappedScope{ std::get<0>(this->parents_), *this, offset, size, this->_vkMapMemory, this->_vkUnmapMemory };
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// DeviceMemory::MappedScope class implementation
//
///////////////////////////////////////////////////////////////////////////////////////////////////

vkwp::DeviceMemory::MappedScope::MappedScope(const VkDevice &       device,
                                             const VkDeviceMemory & device_memory,
                                             const std::size_t      offset,
                                             const std::size_t      size,
                                             PFN_vkMapMemory        map_fn,
                                             PFN_vkUnmapMemory      unmap_fn)
    : device_{ device }
    , memory_{ device_memory }
    , size_{ size }
    , ptr_{ nullptr }
    , _vkUnmapMemory{ unmap_fn }
{
    vkwp::VulkanError::check(  //
        map_fn(device, device_memory, offset, size, 0, &this->ptr_),
        "Failed to vkMapMemory().");
}

vkwp::DeviceMemory::MappedScope::MappedScope() noexcept
    : device_{ nullptr }
    , memory_{ nullptr }
    , size_{ 0 }
    , ptr_{ nullptr }
    , _vkUnmapMemory{ nullptr }
{}

vkwp::DeviceMemory::MappedScope::MappedScope(MappedScope && m) noexcept
    : device_{ m.device_ }
    , memory_{ m.memory_ }
    , size_{ m.size_ }
    , ptr_{ m.ptr_ }
    , _vkUnmapMemory{ m._vkUnmapMemory }
{
    m.device_        = nullptr;
    m.memory_        = nullptr;
    m.size_          = 0;
    m.ptr_           = nullptr;
    m._vkUnmapMemory = nullptr;
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
        this->device_        = rhs.device_;
        this->memory_        = rhs.memory_;
        this->size_          = rhs.size_;
        this->ptr_           = rhs.ptr_;
        this->_vkUnmapMemory = rhs._vkUnmapMemory;
        rhs.device_          = nullptr;
        rhs.memory_          = nullptr;
        rhs.size_            = 0;
        rhs.ptr_             = nullptr;
        rhs._vkUnmapMemory   = nullptr;
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
        this->_vkUnmapMemory(this->device_, this->memory_);
        this->device_        = nullptr;
        this->memory_        = nullptr;
        this->size_          = 0;
        this->ptr_           = nullptr;
        this->_vkUnmapMemory = nullptr;
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
