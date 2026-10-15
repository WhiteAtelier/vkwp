// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#include "vkwp/physical_device.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"

#include "vkwp/instance.hpp"
#include "vkwp/vulkan_error.hpp"
#include "vkwp/vulkan_functions.hpp"

#include <limits>

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete(VkPhysicalDevice, std::nullptr_t, const std::tuple<> &) noexcept
{}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// PhysicalDevice class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
vkwp::PhysicalDevice::PhysicalDevice(std::string && label, VkPhysicalDeviceProperties && pd_properties)
    : Resource{ std::move(label) }
    , properties_{ std::move(pd_properties) }
{}

vkwp::PhysicalDevice::PhysicalDevice(std::string &&                label,
                                     const VkPhysicalDevice        external,
                                     VkPhysicalDeviceProperties && pd_properties)
    : Resource{ std::move(label), external }
    , properties_{ std::move(pd_properties) }
{}

std::vector<vkwp::PhysicalDevice>
vkwp::PhysicalDevice::enumeratePhysicalDevices(const Instance & instance)
{
    return PhysicalDevice::enumeratePhysicalDevices(instance, {});
}

std::vector<vkwp::PhysicalDevice>
vkwp::PhysicalDevice::enumeratePhysicalDevices(const Instance & instance, const OptionalEnumerateArgs & opt_args)
{
    VKWP_VALIDATE_ARG(instance);

    std::vector<VkPhysicalDevice> phys_devs;
    std::uint32_t                 count = 0;

    auto * const enumerate_fn
        = (opt_args.functions != nullptr && opt_args.functions->vkEnumeratePhysicalDevices != nullptr)
            ? opt_args.functions->vkEnumeratePhysicalDevices
            : vkEnumeratePhysicalDevices;
    auto * const get_properties_fn
        = (opt_args.functions != nullptr && opt_args.functions->vkGetPhysicalDeviceProperties != nullptr)
            ? opt_args.functions->vkGetPhysicalDeviceProperties
            : vkGetPhysicalDeviceProperties;

    constexpr std::uint32_t max_attempt_count = 10;
    for (std::uint32_t attempt_count = 0; attempt_count < max_attempt_count; ++attempt_count)
    {
        VulkanError::check(  //
            enumerate_fn(instance, &count, nullptr),
            "Failed to vkEnumeratePhysicalDevice(). (Get array count.)");
        phys_devs.resize(count);
        const VkResult result = enumerate_fn(instance, &count, phys_devs.data());
        if (result == VK_INCOMPLETE && attempt_count + 1 < max_attempt_count)
        {
            continue;
        }
        VulkanError::check(result, "Failed to vkEnumeratePhysicalDevice(). (Get array data.)");
        phys_devs.resize(count);
        break;
    }

    std::vector<vkwp::PhysicalDevice> ret;
    ret.reserve(count);

    for (const auto & phys_dev : phys_devs)
    {
        VkPhysicalDeviceProperties prop;
        get_properties_fn(phys_dev, &prop);

        auto label = "/PhysicalDevice/ID" + std::to_string(prop.deviceID)  //
                   + "/" + std::string{ prop.deviceName };
        auto & pdev           = ret.emplace_back(PhysicalDevice{ std::move(label), std::move(prop) });
        *pdev._reset(nullptr) = phys_dev;
        if (opt_args.functions != nullptr)
        {
            pdev._vkGetPhysicalDeviceQueueFamilyProperties
                = (opt_args.functions->vkGetPhysicalDeviceQueueFamilyProperties != nullptr)
                    ? opt_args.functions->vkGetPhysicalDeviceQueueFamilyProperties
                    : vkGetPhysicalDeviceQueueFamilyProperties;
            pdev._vkGetPhysicalDeviceMemoryProperties
                = (opt_args.functions->vkGetPhysicalDeviceMemoryProperties != nullptr)
                    ? opt_args.functions->vkGetPhysicalDeviceMemoryProperties
                    : vkGetPhysicalDeviceMemoryProperties;
            pdev._vkEnumerateDeviceExtensionProperties
                = (opt_args.functions->vkEnumerateDeviceExtensionProperties != nullptr)
                    ? opt_args.functions->vkEnumerateDeviceExtensionProperties
                    : vkEnumerateDeviceExtensionProperties;
            pdev._vkGetPhysicalDeviceFormatProperties
                = (opt_args.functions->vkGetPhysicalDeviceFormatProperties != nullptr)
                    ? opt_args.functions->vkGetPhysicalDeviceFormatProperties
                    : vkGetPhysicalDeviceFormatProperties;
            pdev._vkGetPhysicalDeviceSurfaceFormatsKHR
                = (opt_args.functions->vkGetPhysicalDeviceSurfaceFormatsKHR != nullptr)
                    ? opt_args.functions->vkGetPhysicalDeviceSurfaceFormatsKHR
                    : vkGetPhysicalDeviceSurfaceFormatsKHR;
            pdev._vkGetPhysicalDeviceSurfaceCapabilitiesKHR
                = (opt_args.functions->vkGetPhysicalDeviceSurfaceCapabilitiesKHR != nullptr)
                    ? opt_args.functions->vkGetPhysicalDeviceSurfaceCapabilitiesKHR
                    : vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
            pdev._vkGetPhysicalDeviceSurfaceSupportKHR
                = (opt_args.functions->vkGetPhysicalDeviceSurfaceSupportKHR != nullptr)
                    ? opt_args.functions->vkGetPhysicalDeviceSurfaceSupportKHR
                    : vkGetPhysicalDeviceSurfaceSupportKHR;
        }
        else
        {
            pdev._vkGetPhysicalDeviceQueueFamilyProperties  = vkGetPhysicalDeviceQueueFamilyProperties;
            pdev._vkGetPhysicalDeviceMemoryProperties       = vkGetPhysicalDeviceMemoryProperties;
            pdev._vkEnumerateDeviceExtensionProperties      = vkEnumerateDeviceExtensionProperties;
            pdev._vkGetPhysicalDeviceFormatProperties       = vkGetPhysicalDeviceFormatProperties;
            pdev._vkGetPhysicalDeviceSurfaceFormatsKHR      = vkGetPhysicalDeviceSurfaceFormatsKHR;
            pdev._vkGetPhysicalDeviceSurfaceCapabilitiesKHR = vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
            pdev._vkGetPhysicalDeviceSurfaceSupportKHR      = vkGetPhysicalDeviceSurfaceSupportKHR;
        }
    }
    return ret;
}

vkwp::PhysicalDevice
vkwp::PhysicalDevice::importExternal(std::string                   label,
                                     const VkPhysicalDevice        external,
                                     const VulkanFunctions * const functions)
{
    VKWP_VALIDATE_ARG(external);

    auto * const get_properties_fn = (functions != nullptr && functions->vkGetPhysicalDeviceProperties != nullptr)
                                       ? functions->vkGetPhysicalDeviceProperties
                                       : vkGetPhysicalDeviceProperties;
    VkPhysicalDeviceProperties properties;
    get_properties_fn(external, &properties);

    PhysicalDevice ret{ std::move(label), external, std::move(properties) };
    ret._vkGetPhysicalDeviceQueueFamilyProperties
        = (functions != nullptr && functions->vkGetPhysicalDeviceQueueFamilyProperties != nullptr)
            ? functions->vkGetPhysicalDeviceQueueFamilyProperties
            : vkGetPhysicalDeviceQueueFamilyProperties;
    ret._vkGetPhysicalDeviceMemoryProperties
        = (functions != nullptr && functions->vkGetPhysicalDeviceMemoryProperties != nullptr)
            ? functions->vkGetPhysicalDeviceMemoryProperties
            : vkGetPhysicalDeviceMemoryProperties;
    ret._vkEnumerateDeviceExtensionProperties
        = (functions != nullptr && functions->vkEnumerateDeviceExtensionProperties != nullptr)
            ? functions->vkEnumerateDeviceExtensionProperties
            : vkEnumerateDeviceExtensionProperties;
    ret._vkGetPhysicalDeviceFormatProperties
        = (functions != nullptr && functions->vkGetPhysicalDeviceFormatProperties != nullptr)
            ? functions->vkGetPhysicalDeviceFormatProperties
            : vkGetPhysicalDeviceFormatProperties;
    ret._vkGetPhysicalDeviceSurfaceFormatsKHR
        = (functions != nullptr && functions->vkGetPhysicalDeviceSurfaceFormatsKHR != nullptr)
            ? functions->vkGetPhysicalDeviceSurfaceFormatsKHR
            : vkGetPhysicalDeviceSurfaceFormatsKHR;
    ret._vkGetPhysicalDeviceSurfaceCapabilitiesKHR
        = (functions != nullptr && functions->vkGetPhysicalDeviceSurfaceCapabilitiesKHR != nullptr)
            ? functions->vkGetPhysicalDeviceSurfaceCapabilitiesKHR
            : vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
    ret._vkGetPhysicalDeviceSurfaceSupportKHR
        = (functions != nullptr && functions->vkGetPhysicalDeviceSurfaceSupportKHR != nullptr)
            ? functions->vkGetPhysicalDeviceSurfaceSupportKHR
            : vkGetPhysicalDeviceSurfaceSupportKHR;
    return ret;
}

const std::vector<VkQueueFamilyProperties> &
vkwp::PhysicalDevice::getQueueFamilyProperties() const
{
    VKWP_VALIDATE_THIS;
    if (!this->queue_family_properties_cache_)
    {
        this->queue_family_properties_cache_.emplace();
        {
            uint32_t queue_family_properties_count{ 0 };
            this->_vkGetPhysicalDeviceQueueFamilyProperties(*this, &queue_family_properties_count, nullptr);
            this->queue_family_properties_cache_->resize(queue_family_properties_count);

            this->_vkGetPhysicalDeviceQueueFamilyProperties(*this,
                                                            &queue_family_properties_count,
                                                            this->queue_family_properties_cache_->data());
        }
    }
    return *this->queue_family_properties_cache_;
}

const VkPhysicalDeviceProperties &
vkwp::PhysicalDevice::getProperties() const noexcept
{
    return this->properties_;
}

const VkPhysicalDeviceMemoryProperties &
vkwp::PhysicalDevice::getMemoryProperties() const
{
    VKWP_VALIDATE_THIS;
    if (!this->memory_properties_)
    {
        this->memory_properties_.emplace();
        this->_vkGetPhysicalDeviceMemoryProperties(*this, &*this->memory_properties_);
    }
    return *this->memory_properties_;
}

std::vector<VkExtensionProperties>
vkwp::PhysicalDevice::enumerateDeviceExtensionProperties() const
{
    VKWP_VALIDATE_THIS;
    std::vector<VkExtensionProperties> ret;
    constexpr std::uint32_t            max_attempt_count = 10;
    for (std::uint32_t attempt_count = 0; attempt_count < max_attempt_count; ++attempt_count)
    {
        std::uint32_t count = 0;
        VulkanError::check(  //
            this->_vkEnumerateDeviceExtensionProperties(*this, nullptr, &count, nullptr),
            "Failed to vkEnumerateDeviceExtensionProperties(). (array size)");
        ret.resize(count);
        const VkResult result = this->_vkEnumerateDeviceExtensionProperties(*this, nullptr, &count, ret.data());
        if (result == VK_INCOMPLETE && attempt_count + 1 < max_attempt_count)
        {
            continue;
        }
        VulkanError::check(result, "Failed to vkEnumerateDeviceExtensionProperties(). (array data)");
        ret.resize(count);
        return ret;
    }
    throw VulkanError{ VK_INCOMPLETE, "Failed to vkEnumerateDeviceExtensionProperties() after maximum attempts." };
}

VkFormatProperties
vkwp::PhysicalDevice::getFormatProperties(const VkFormat format) const
{
    VKWP_VALIDATE_THIS;
    VkFormatProperties prop;
    this->_vkGetPhysicalDeviceFormatProperties(*this, format, &prop);
    return prop;
}

std::vector<VkSurfaceFormatKHR>
vkwp::PhysicalDevice::getSurfaceFormats(const VkSurfaceKHR & surface) const
{
    VKWP_VALIDATE_THIS;
    std::vector<VkSurfaceFormatKHR> formats;
    constexpr std::uint32_t         max_attempt_count = 10;
    for (std::uint32_t attempt_count = 0; attempt_count < max_attempt_count; ++attempt_count)
    {
        std::uint32_t count = 0;
        VulkanError::check(  //
            this->_vkGetPhysicalDeviceSurfaceFormatsKHR(*this, surface, &count, nullptr),
            "Failed to vkGetPhysicalDeviceSurfaceFormatsKHR(). (array count)");
        formats.resize(count);
        const VkResult result = this->_vkGetPhysicalDeviceSurfaceFormatsKHR(*this, surface, &count, formats.data());
        if (result == VK_INCOMPLETE && attempt_count + 1 < max_attempt_count)
        {
            continue;
        }
        VulkanError::check(result, "Failed to vkGetPhysicalDeviceSurfaceFormatsKHR(). (array data)");
        formats.resize(count);
        return formats;
    }
    throw VulkanError{ VK_INCOMPLETE, "Failed to vkGetPhysicalDeviceSurfaceFormatsKHR() after maximum attempts." };
}

VkSurfaceCapabilitiesKHR
vkwp::PhysicalDevice::getSurfaceCapabilities(const VkSurfaceKHR & surface) const
{
    VKWP_VALIDATE_THIS;
    VkSurfaceCapabilitiesKHR ret;
    VulkanError::check(  //
        this->_vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*this, surface, &ret),
        "Failed to vkGetPhysicalDeviceSurfaceCapabilitiesKHR().");
    return ret;
}

std::pair<bool, std::uint32_t>
vkwp::PhysicalDevice::getMemoryTypeIndex(const VkMemoryPropertyFlags memory_property_flags,
                                         const std::uint32_t         memory_type_bits) const
{
    // vulkan resource に直接アクセスしないため, validate しないが, getMemoryProperties() の中で validate される.
    return getMemoryTypeIndex(this->getMemoryProperties(), memory_property_flags, memory_type_bits);
}

std::pair<bool, std::uint32_t>
vkwp::PhysicalDevice::getMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties & memory_properties,
                                         const VkMemoryPropertyFlags              memory_property_flags,
                                         const std::uint32_t                      memory_type_bits)
{
    auto request_bits = memory_type_bits;
    for (std::uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
    {
        if (request_bits & 1)
        {
            const auto & types = memory_properties.memoryTypes[i];
            if ((types.propertyFlags & memory_property_flags) == memory_property_flags)
            {
                return { true, i };
            }
        }
        request_bits >>= 1;
    }
    return { false, ~0u };
}

std::set<std::uint32_t>
vkwp::PhysicalDevice::listQueueFamilyIndexes(const VkQueueFlags & queue_flags) const
{
    // vulkan resource に直接アクセスしないため, validate しないが, getQueueFamilyProperties() の中で validate される.
    const auto &            queue_family_properties = this->getQueueFamilyProperties();
    std::set<std::uint32_t> indexes;
    for (std::uint32_t queue_family_index = 0;  //
         queue_family_index < queue_family_properties.size();
         ++queue_family_index)
    {
        if ((queue_family_properties[queue_family_index].queueFlags & queue_flags) == queue_flags)
        {
            indexes.insert(queue_family_index);
        }
    }
    return indexes;
}

bool
vkwp::PhysicalDevice::isSurfaceSupported(const std::uint32_t queue_index, const VkSurfaceKHR & surface) const
{
    VKWP_VALIDATE_THIS;
    VkBool32 supported = VK_FALSE;
    vkwp::VulkanError::check(  //
        this->_vkGetPhysicalDeviceSurfaceSupportKHR(*this, queue_index, surface, &supported),
        "Failed to vkGetPhysicalDeviceSurfaceSupportKHR().");
    return supported == VK_TRUE;
}
