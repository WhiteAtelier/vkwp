#include "vkwp/physical_device.hpp"

#include "impl/assert.hpp"
#include "vkwp/device.hpp"
#include "vkwp/vulkan_error.hpp"

#include <limits>

vkwp::PhysicalDevice::PhysicalDevice() = default;

vkwp::PhysicalDevice::PhysicalDevice(const std::string & label)
    : Resource{ label }
{}

const std::vector<VkQueueFamilyProperties> &
vkwp::PhysicalDevice::getQueueFamilyProperties() const
{
    if (!this->queue_family_properties_cache_)
    {
        this->queue_family_properties_cache_.emplace();
        {
            uint32_t queue_family_properties_count{ 0 };
            vkGetPhysicalDeviceQueueFamilyProperties(this->_ref(), &queue_family_properties_count, nullptr);
            this->queue_family_properties_cache_->resize(queue_family_properties_count);

            vkGetPhysicalDeviceQueueFamilyProperties(this->_ref(),
                                                     &queue_family_properties_count,
                                                     this->queue_family_properties_cache_->data());
        }
    }
    return *this->queue_family_properties_cache_;
}

const VkPhysicalDeviceProperties &
vkwp::PhysicalDevice::getProperties() const
{
    if (!this->properties_)
    {
        this->properties_.emplace();
        vkGetPhysicalDeviceProperties(this->_ref(), &*this->properties_);
    }
    return *this->properties_;
}

const VkPhysicalDeviceMemoryProperties &
vkwp::PhysicalDevice::getMemoryProperties() const
{
    if (!this->memory_properties_)
    {
        this->memory_properties_.emplace();
        vkGetPhysicalDeviceMemoryProperties(this->_ref(), &*this->memory_properties_);
    }
    return *this->memory_properties_;
}

std::vector<VkExtensionProperties>
vkwp::PhysicalDevice::enumerateDeviceExtensionProperties() const
{
    std::uint32_t count = 0;
    VulkanError::check(  //
        vkEnumerateDeviceExtensionProperties(this->_ref(), nullptr, &count, nullptr),
        "Failed to vkEnumerateDeviceExtensionProperties(). (array size)");
    std::vector<VkExtensionProperties> ret(count);
    VulkanError::check(  //
        vkEnumerateDeviceExtensionProperties(this->_ref(), nullptr, &count, ret.data()),
        "Failed to vkEnumerateDeviceExtensionProperties(). (array data)");
    return ret;
}

VkFormatProperties
vkwp::PhysicalDevice::getFormatProperties(const VkFormat format) const
{
    VkFormatProperties prop;
    vkGetPhysicalDeviceFormatProperties(this->_ref(), format, &prop);
    return prop;
}

vkwp::Device
vkwp::PhysicalDevice::createDevice(const std::string &                          label,
                                          const std::vector<VkDeviceQueueCreateInfo> & queue_create_infos,
                                          const std::vector<std::string> &             _enabled_layer_names,
                                          const std::vector<std::string> &             _enabled_extension_names,
                                          const VkPhysicalDeviceFeatures &             enabled_features,
                                          const void *                                 p_next)
{
    VKWP_ASSERTION(queue_create_infos.size() <= std::numeric_limits<std::uint32_t>::max());
    VKWP_ASSERTION(_enabled_layer_names.size() <= std::numeric_limits<std::uint32_t>::max());
    VKWP_ASSERTION(_enabled_extension_names.size() <= std::numeric_limits<std::uint32_t>::max());
    Device ret{ label };

    std::vector<const char *> enabled_layer_names;
    enabled_layer_names.reserve(_enabled_layer_names.size());
    for (const auto & name : _enabled_layer_names)
    {
        enabled_layer_names.emplace_back(name.c_str());
    }

    std::vector<const char *> enabled_extension_names;
    enabled_extension_names.reserve(_enabled_extension_names.size());
    for (const auto & name : _enabled_extension_names)
    {
        enabled_extension_names.emplace_back(name.c_str());
    }

    const VkDeviceCreateInfo ci{
        .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext                   = p_next,
        .flags                   = 0,
        .queueCreateInfoCount    = static_cast<std::uint32_t>(queue_create_infos.size()),
        .pQueueCreateInfos       = queue_create_infos.data(),
        .enabledLayerCount       = static_cast<std::uint32_t>(enabled_layer_names.size()),
        .ppEnabledLayerNames     = enabled_layer_names.data(),
        .enabledExtensionCount   = static_cast<std::uint32_t>(enabled_extension_names.size()),
        .ppEnabledExtensionNames = enabled_extension_names.data(),
        .pEnabledFeatures        = &enabled_features,
    };
    VulkanError::check(  //
        vkCreateDevice(this->_ref(), &ci, nullptr, ret.reset()),
        "Failed to vkCreateDevice().");
    return ret;
}

std::vector<VkSurfaceFormatKHR>
vkwp::PhysicalDevice::getSurfaceFormats(const VkSurfaceKHR & surface) const
{
    std::uint32_t count = 0;
    VulkanError::check(  //
        vkGetPhysicalDeviceSurfaceFormatsKHR(this->_ref(), surface, &count, nullptr),
        "Failed to vkGetPhysicalDeviceSurfaceFormatsKHR(). (array count)");
    std::vector<VkSurfaceFormatKHR> formats;
    formats.resize(count);
    VulkanError::check(  //
        vkGetPhysicalDeviceSurfaceFormatsKHR(this->_ref(), surface, &count, formats.data()),
        "Failed to vkGetPhysicalDeviceSurfaceFormatsKHR(). (array data)");
    return formats;
}

VkSurfaceCapabilitiesKHR
vkwp::PhysicalDevice::getSurfaceCapabilities(const VkSurfaceKHR & surface) const
{
    VkSurfaceCapabilitiesKHR ret;
    VulkanError::check(  //
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->_ref(), surface, &ret),
        "Failed to vkGetPhysicalDeviceSurfaceCapabilitiesKHR().");
    return ret;
}

std::pair<bool, std::uint32_t>
vkwp::PhysicalDevice::getMemoryTypeIndex(const VkMemoryPropertyFlags memory_property_flags,
                                                const std::uint32_t         memory_type_bits) const
{
    const auto & memory_property = this->getMemoryProperties();
    auto         request_bits    = memory_type_bits;
    for (std::uint32_t i = 0; i < memory_property.memoryTypeCount; ++i)
    {
        if (request_bits & 1)
        {
            const auto & types = memory_property.memoryTypes[i];
            if ((types.propertyFlags & memory_property_flags) == memory_property_flags)
            {
                return { true, i };
            }
        }
        request_bits >>= 1;
    }
    return { false, ~0u };
}
