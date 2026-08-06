#include "vkwp/device.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"

#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/mutex.hpp"
#include "vkwp/queue.hpp"
#include "vkwp/vulkan_error.hpp"
#include "vkwp/vulkan_functions.hpp"

#include <spdlog/spdlog.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete(VkDevice i, PFN_vkDestroyDevice fn, const std::tuple<> &) noexcept
{
    fn(i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Device class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::Device
vkwp::Device::create(std::string                                  label,
                     const VkPhysicalDevice                       physical_device,
                     const std::vector<VkDeviceQueueCreateInfo> & queue_create_infos,
                     const std::vector<std::string> &             enabled_layer_names,
                     const std::vector<std::string> &             enabled_extension_names,
                     const VkPhysicalDeviceFeatures * const       enabled_features)
{
    return Device::create(std::move(label),
                          physical_device,
                          queue_create_infos,
                          enabled_layer_names,
                          enabled_extension_names,
                          enabled_features,
                          {});
}

/* STATIC */
vkwp::Device
vkwp::Device::create(std::string                                  label,
                     const VkPhysicalDevice                       physical_device,
                     const std::vector<VkDeviceQueueCreateInfo> & queue_create_infos,
                     const std::vector<std::string> &             _enabled_layer_names,
                     const std::vector<std::string> &             _enabled_extension_names,
                     const VkPhysicalDeviceFeatures * const       enabled_features,
                     const OptionalCreateArgs &                   opt_args)
{
    VKWP_VALIDATE_ARG(physical_device);
    VKWP_ASSERTION(queue_create_infos.size() <= (std::numeric_limits<std::uint32_t>::max)());
    VKWP_ASSERTION(_enabled_layer_names.size() <= (std::numeric_limits<std::uint32_t>::max)());
    VKWP_ASSERTION(_enabled_extension_names.size() <= (std::numeric_limits<std::uint32_t>::max)());
    Device ret{ std::move(label) };

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
        .pNext                   = opt_args.p_next,
        .flags                   = 0,
        .queueCreateInfoCount    = static_cast<std::uint32_t>(queue_create_infos.size()),
        .pQueueCreateInfos       = queue_create_infos.data(),
        .enabledLayerCount       = static_cast<std::uint32_t>(enabled_layer_names.size()),
        .ppEnabledLayerNames     = enabled_layer_names.data(),
        .enabledExtensionCount   = static_cast<std::uint32_t>(enabled_extension_names.size()),
        .ppEnabledExtensionNames = enabled_extension_names.data(),
        .pEnabledFeatures        = enabled_features,
    };
    auto * const create_fn  = (opt_args.functions != nullptr && opt_args.functions->vkCreateDevice != nullptr)
                                ? opt_args.functions->vkCreateDevice
                                : vkCreateDevice;
    auto * const destroy_fn = (opt_args.functions != nullptr && opt_args.functions->vkDestroyDevice != nullptr)
                                ? opt_args.functions->vkDestroyDevice
                                : vkDestroyDevice;

    VulkanError::check(create_fn(physical_device, &ci, nullptr, ret._reset(destroy_fn)),  //
                       "Failed to vkCreateDevice().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(ret,
                                                         VK_OBJECT_TYPE_DEVICE,
                                                         static_cast<VkDevice>(ret),
                                                         ret.getLabel());
    }
    if (opt_args.functions != nullptr)
    {
        ret._vkDeviceWaitIdle = (opt_args.functions->vkDeviceWaitIdle != nullptr) ? opt_args.functions->vkDeviceWaitIdle
                                                                                  : vkDeviceWaitIdle;
        ret._vkGetDeviceQueue = (opt_args.functions->vkGetDeviceQueue != nullptr) ? opt_args.functions->vkGetDeviceQueue
                                                                                  : vkGetDeviceQueue;
    }
    return ret;
}

void
vkwp::Device::waitIdle() const
{
    VKWP_VALIDATE_THIS;
    VulkanError::check(this->_vkDeviceWaitIdle(*this), "Failed to vkDeviceWaitIdle().");
}

vkwp::Queue
vkwp::Device::getDeviceQueue(const std::uint32_t queue_family_index,  //
                             const std::uint32_t queue_index) const
{
    VKWP_VALIDATE_THIS;
    return this->getDeviceQueue(queue_family_index, queue_index, {}, {});
}

vkwp::Queue
vkwp::Device::getDeviceQueue(const std::uint32_t       queue_family_index,
                             const std::uint32_t       queue_index,
                             std::unique_ptr<Mutex> && mutex) const
{
    VKWP_VALIDATE_THIS;
    return this->getDeviceQueue(queue_family_index, queue_index, std::move(mutex), {});
}

vkwp::Queue
vkwp::Device::getDeviceQueue(const std::uint32_t        queue_family_index,
                             const std::uint32_t        queue_index,
                             std::unique_ptr<Mutex> &&  mutex,
                             const DeviceQueueGetArgs & opt_args) const
{
    VKWP_VALIDATE_THIS;
    Queue ret{ this->getLabel() + "/Queue"                        //
               + "/Family[" + std::to_string(queue_family_index)  //
               + "]/Index[" + std::to_string(queue_index) + "]" };
    this->_vkGetDeviceQueue(*this, queue_family_index, queue_index, ret._reset(nullptr));
    ret.queue_family_index_ = queue_family_index;
    ret.mutex_              = std::move(mutex);
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(*this,
                                                         VK_OBJECT_TYPE_QUEUE,
                                                         static_cast<VkQueue>(ret),
                                                         ret.getLabel());
    }
    return ret;
}
