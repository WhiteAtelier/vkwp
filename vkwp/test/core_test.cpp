#include "test_utils.hpp"

#include <gtest/gtest.h>
#include <vkwp/device.hpp>
#include <vkwp/instance.hpp>
#include <vkwp/physical_device.hpp>
#include <vkwp/queue.hpp>
#include <vkwp/vulkan_error.hpp>
#include <vkwp/vulkan_functions.hpp>

#include <array>
#include <cstring>
#include <set>
#include <string>
#include <vector>

namespace {

using vkwp::test::fakeHandle;
using vkwp::test::fakePointer;

struct CoreState
{
    int              create_instance_calls  = 0;
    int              destroy_instance_calls = 0;
    int              create_device_calls    = 0;
    int              destroy_device_calls   = 0;
    int              wait_idle_calls        = 0;
    int              get_queue_calls        = 0;
    int              enumerate_calls        = 0;
    VkInstance       instance               = fakeHandle<VkInstance>(0x101);
    VkPhysicalDevice physical_device        = fakeHandle<VkPhysicalDevice>(0x201);
    VkDevice         device                 = fakeHandle<VkDevice>(0x301);
    VkQueue          queue                  = fakeHandle<VkQueue>(0x401);
};

CoreState state;

VkResult VKAPI_CALL
createInstance(const VkInstanceCreateInfo * create_info, const VkAllocationCallbacks * allocator, VkInstance * instance)
{
    ++state.create_instance_calls;
    EXPECT_EQ(allocator, nullptr);
    EXPECT_NE(create_info, nullptr);
    EXPECT_EQ(create_info->sType, VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO);
    EXPECT_EQ(create_info->pNext, fakePointer(0x501));
    EXPECT_NE(create_info->pApplicationInfo, nullptr);
    EXPECT_STREQ(create_info->pApplicationInfo->pApplicationName, "application");
    EXPECT_EQ(create_info->pApplicationInfo->applicationVersion, 12u);
    EXPECT_STREQ(create_info->pApplicationInfo->pEngineName, "engine");
    EXPECT_EQ(create_info->pApplicationInfo->engineVersion, 34u);
    EXPECT_EQ(create_info->pApplicationInfo->apiVersion, VK_API_VERSION_1_3);
    EXPECT_EQ(create_info->enabledLayerCount, 1u);
    EXPECT_STREQ(create_info->ppEnabledLayerNames[0], "layer");
    EXPECT_EQ(create_info->enabledExtensionCount, 1u);
    EXPECT_STREQ(create_info->ppEnabledExtensionNames[0], "extension");
    *instance = state.instance;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroyInstance(const VkInstance instance, const VkAllocationCallbacks * allocator)
{
    ++state.destroy_instance_calls;
    EXPECT_EQ(instance, state.instance);
    EXPECT_EQ(allocator, nullptr);
}

VkResult VKAPI_CALL
enumerateInstanceExtensions(const char * layer_name, std::uint32_t * property_count, VkExtensionProperties * properties)
{
    EXPECT_EQ(layer_name, nullptr);
    EXPECT_NE(property_count, nullptr);
    if (properties == nullptr)
    {
        *property_count = 1;
    }
    else
    {
        EXPECT_EQ(*property_count, 1u);
        std::memcpy(properties[0].extensionName, "VK_TEST_extension", sizeof("VK_TEST_extension"));
        properties[0].specVersion = 7;
    }
    return VK_SUCCESS;
}

int incomplete_instance_extension_calls = 0;

VkResult VKAPI_CALL
enumerateChangingInstanceExtensions(const char *, std::uint32_t * property_count, VkExtensionProperties * properties)
{
    ++incomplete_instance_extension_calls;
    if (properties == nullptr)
    {
        *property_count = incomplete_instance_extension_calls == 1 ? 1u : 2u;
        return VK_SUCCESS;
    }
    if (incomplete_instance_extension_calls == 2)
    {
        *property_count = 1;
        return VK_INCOMPLETE;
    }
    std::memcpy(properties[0].extensionName, "VK_TEST_first", sizeof("VK_TEST_first"));
    std::memcpy(properties[1].extensionName, "VK_TEST_second", sizeof("VK_TEST_second"));
    *property_count = 2;
    return VK_SUCCESS;
}

int incomplete_instance_extension_forever_calls = 0;

VkResult VKAPI_CALL
enumerateIncompleteInstanceExtensions(const char *, std::uint32_t * property_count, VkExtensionProperties * properties)
{
    ++incomplete_instance_extension_forever_calls;
    *property_count = 1;
    return properties == nullptr ? VK_SUCCESS : VK_INCOMPLETE;
}

VkResult VKAPI_CALL
createDevice(const VkPhysicalDevice        physical_device,
             const VkDeviceCreateInfo *    create_info,
             const VkAllocationCallbacks * allocator,
             VkDevice *                    device)
{
    ++state.create_device_calls;
    EXPECT_EQ(physical_device, state.physical_device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_NE(create_info, nullptr);
    EXPECT_EQ(create_info->pNext, fakePointer(0x502));
    EXPECT_EQ(create_info->queueCreateInfoCount, 1u);
    EXPECT_EQ(create_info->pQueueCreateInfos[0].queueFamilyIndex, 3u);
    EXPECT_EQ(create_info->pQueueCreateInfos[0].queueCount, 1u);
    EXPECT_EQ(create_info->enabledLayerCount, 1u);
    EXPECT_STREQ(create_info->ppEnabledLayerNames[0], "device-layer");
    EXPECT_EQ(create_info->enabledExtensionCount, 1u);
    EXPECT_STREQ(create_info->ppEnabledExtensionNames[0], "device-extension");
    EXPECT_NE(create_info->pEnabledFeatures, nullptr);
    EXPECT_EQ(create_info->pEnabledFeatures->robustBufferAccess, VK_TRUE);
    *device = state.device;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroyDevice(const VkDevice device, const VkAllocationCallbacks * allocator)
{
    ++state.destroy_device_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(allocator, nullptr);
}

VkResult VKAPI_CALL
deviceWaitIdle(const VkDevice device)
{
    ++state.wait_idle_calls;
    EXPECT_EQ(device, state.device);
    return VK_SUCCESS;
}

void VKAPI_CALL
getDeviceQueue(const VkDevice      device,
               const std::uint32_t queue_family_index,
               const std::uint32_t queue_index,
               VkQueue *           queue)
{
    ++state.get_queue_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(queue_family_index, 3u);
    EXPECT_EQ(queue_index, 2u);
    *queue = state.queue;
}

VkResult VKAPI_CALL
enumeratePhysicalDevices(const VkInstance instance, std::uint32_t * count, VkPhysicalDevice * physical_devices)
{
    ++state.enumerate_calls;
    EXPECT_EQ(instance, state.instance);
    EXPECT_NE(count, nullptr);
    if (physical_devices == nullptr)
    {
        *count = 1;
    }
    else
    {
        EXPECT_EQ(*count, 1u);
        physical_devices[0] = state.physical_device;
    }
    return VK_SUCCESS;
}

void VKAPI_CALL
getPhysicalDeviceProperties(const VkPhysicalDevice physical_device, VkPhysicalDeviceProperties * properties)
{
    EXPECT_EQ(physical_device, state.physical_device);
    *properties            = {};
    properties->deviceID   = 77;
    properties->apiVersion = VK_API_VERSION_1_3;
    std::memcpy(properties->deviceName, "Mock GPU", sizeof("Mock GPU"));
}

void VKAPI_CALL
getQueueFamilyProperties(const VkPhysicalDevice    physical_device,
                         std::uint32_t *           count,
                         VkQueueFamilyProperties * properties)
{
    EXPECT_EQ(physical_device, state.physical_device);
    ASSERT_NE(count, nullptr);
    if (properties == nullptr)
    {
        *count = 2;
    }
    else
    {
        ASSERT_EQ(*count, 2u);
        properties[0]            = {};
        properties[0].queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
        properties[0].queueCount = 2;
        properties[1]            = {};
        properties[1].queueFlags = VK_QUEUE_TRANSFER_BIT;
        properties[1].queueCount = 1;
    }
}

void VKAPI_CALL
getMemoryProperties(const VkPhysicalDevice physical_device, VkPhysicalDeviceMemoryProperties * properties)
{
    EXPECT_EQ(physical_device, state.physical_device);
    *properties                              = {};
    properties->memoryTypeCount              = 2;
    properties->memoryTypes[0].propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    properties->memoryTypes[1].propertyFlags
        = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
}

VkResult VKAPI_CALL
enumerateDeviceExtensions(const VkPhysicalDevice  physical_device,
                          const char *            layer_name,
                          std::uint32_t *         count,
                          VkExtensionProperties * properties)
{
    EXPECT_EQ(physical_device, state.physical_device);
    EXPECT_EQ(layer_name, nullptr);
    if (properties == nullptr)
    {
        *count = 1;
    }
    else
    {
        std::memcpy(properties[0].extensionName, "VK_TEST_device", sizeof("VK_TEST_device"));
        properties[0].specVersion = 9;
    }
    return VK_SUCCESS;
}

void VKAPI_CALL
getFormatProperties(const VkPhysicalDevice physical_device, const VkFormat format, VkFormatProperties * properties)
{
    EXPECT_EQ(physical_device, state.physical_device);
    EXPECT_EQ(format, VK_FORMAT_R8G8B8A8_UNORM);
    *properties                       = {};
    properties->optimalTilingFeatures = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
}

VkResult VKAPI_CALL
getSurfaceFormats(const VkPhysicalDevice physical_device,
                  const VkSurfaceKHR     surface,
                  std::uint32_t *        count,
                  VkSurfaceFormatKHR *   formats)
{
    EXPECT_EQ(physical_device, state.physical_device);
    EXPECT_EQ(surface, fakeHandle<VkSurfaceKHR>(0x601));
    if (formats == nullptr)
    {
        *count = 1;
    }
    else
    {
        formats[0] = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    }
    return VK_SUCCESS;
}

VkResult VKAPI_CALL
getSurfaceCapabilities(const VkPhysicalDevice     physical_device,
                       const VkSurfaceKHR         surface,
                       VkSurfaceCapabilitiesKHR * capabilities)
{
    EXPECT_EQ(physical_device, state.physical_device);
    EXPECT_EQ(surface, fakeHandle<VkSurfaceKHR>(0x601));
    *capabilities               = {};
    capabilities->minImageCount = 2;
    return VK_SUCCESS;
}

VkResult VKAPI_CALL
getSurfaceSupport(const VkPhysicalDevice physical_device,
                  const std::uint32_t    queue_family_index,
                  const VkSurfaceKHR     surface,
                  VkBool32 *             supported)
{
    EXPECT_EQ(physical_device, state.physical_device);
    EXPECT_EQ(queue_family_index, 1u);
    EXPECT_EQ(surface, fakeHandle<VkSurfaceKHR>(0x601));
    *supported = VK_TRUE;
    return VK_SUCCESS;
}

}  // namespace

TEST(VulkanFunctionsCoreTest, InstanceForwardsCreateArgumentsAndDestroys)
{
    state = {};
    vkwp::VulkanFunctions functions;
    functions.vkCreateInstance  = &createInstance;
    functions.vkDestroyInstance = &destroyInstance;

    {
        vkwp::Instance::OptionalCreateArgs optional_args{
            .p_next    = fakePointer(0x501),
            .functions = &functions,
        };
        auto instance = vkwp::Instance::create("instance",
                                               "application",
                                               12,
                                               "engine",
                                               34,
                                               VK_API_VERSION_1_3,
                                               { "layer" },
                                               { "extension" },
                                               optional_args);
        EXPECT_EQ(static_cast<VkInstance>(instance), state.instance);
        EXPECT_EQ(state.create_instance_calls, 1);
        EXPECT_EQ(state.destroy_instance_calls, 0);
    }
    EXPECT_EQ(state.destroy_instance_calls, 1);
}

TEST(VulkanFunctionsCoreTest, EnumerateInstanceExtensionsUsesInjectedFunction)
{
    vkwp::VulkanFunctions functions;
    functions.vkEnumerateInstanceExtensionProperties = &enumerateInstanceExtensions;

    const auto extensions = vkwp::Instance::enumerateInstanceExtensionProperties(&functions);
    ASSERT_EQ(extensions.size(), 1u);
    EXPECT_STREQ(extensions[0].extensionName, "VK_TEST_extension");
    EXPECT_EQ(extensions[0].specVersion, 7u);
}

TEST(VulkanFunctionsCoreTest, DeviceForwardsArgumentsCachesOperationsAndDestroys)
{
    state = {};
    vkwp::VulkanFunctions functions;
    functions.vkCreateDevice   = &createDevice;
    functions.vkDestroyDevice  = &destroyDevice;
    functions.vkDeviceWaitIdle = &deviceWaitIdle;
    functions.vkGetDeviceQueue = &getDeviceQueue;

    const float                   priority = 0.75f;
    const VkDeviceQueueCreateInfo queue_create_info{
        .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = 0,
        .queueFamilyIndex = 3,
        .queueCount       = 1,
        .pQueuePriorities = &priority,
    };
    VkPhysicalDeviceFeatures features{};
    features.robustBufferAccess = VK_TRUE;

    {
        vkwp::Device::OptionalCreateArgs optional_args{
            .p_next      = fakePointer(0x502),
            .debug_utils = nullptr,
            .functions   = &functions,
        };
        auto device = vkwp::Device::create("device",
                                           state.physical_device,
                                           { queue_create_info },
                                           { "device-layer" },
                                           { "device-extension" },
                                           &features,
                                           optional_args);
        device.waitIdle();
        const auto queue = device.getDeviceQueue(3, 2);
        EXPECT_EQ(static_cast<VkQueue>(queue), state.queue);
        EXPECT_EQ(queue.getQueueFamilyIndex(), 3u);
        EXPECT_EQ(state.wait_idle_calls, 1);
        EXPECT_EQ(state.get_queue_calls, 1);
        EXPECT_EQ(state.destroy_device_calls, 0);
    }
    EXPECT_EQ(state.destroy_device_calls, 1);
}

TEST(VulkanFunctionsCoreTest, EnumerateInstanceExtensionsRetriesIncompleteResult)
{
    incomplete_instance_extension_calls = 0;
    vkwp::VulkanFunctions functions;
    functions.vkEnumerateInstanceExtensionProperties = &enumerateChangingInstanceExtensions;

    const auto extensions = vkwp::Instance::enumerateInstanceExtensionProperties(&functions);
    ASSERT_EQ(extensions.size(), 2u);
    EXPECT_STREQ(extensions[0].extensionName, "VK_TEST_first");
    EXPECT_STREQ(extensions[1].extensionName, "VK_TEST_second");
    EXPECT_EQ(incomplete_instance_extension_calls, 4);
}

TEST(VulkanFunctionsCoreTest, EnumerateInstanceExtensionsThrowsAfterTenIncompleteResults)
{
    incomplete_instance_extension_forever_calls = 0;
    vkwp::VulkanFunctions functions;
    functions.vkEnumerateInstanceExtensionProperties = &enumerateIncompleteInstanceExtensions;

    try
    {
        static_cast<void>(vkwp::Instance::enumerateInstanceExtensionProperties(&functions));
        FAIL() << "VulkanError was not thrown.";
    }
    catch (const vkwp::VulkanError & error)
    {
        EXPECT_EQ(error.result(), VK_INCOMPLETE);
    }
    EXPECT_EQ(incomplete_instance_extension_forever_calls, 20);
}

TEST(VulkanFunctionsCoreTest, PhysicalDeviceCachesAndForwardsQueryFunctions)
{
    state = {};
    vkwp::VulkanFunctions functions;
    functions.vkEnumeratePhysicalDevices                = &enumeratePhysicalDevices;
    functions.vkGetPhysicalDeviceProperties             = &getPhysicalDeviceProperties;
    functions.vkGetPhysicalDeviceQueueFamilyProperties  = &getQueueFamilyProperties;
    functions.vkGetPhysicalDeviceMemoryProperties       = &getMemoryProperties;
    functions.vkEnumerateDeviceExtensionProperties      = &enumerateDeviceExtensions;
    functions.vkGetPhysicalDeviceFormatProperties       = &getFormatProperties;
    functions.vkGetPhysicalDeviceSurfaceFormatsKHR      = &getSurfaceFormats;
    functions.vkGetPhysicalDeviceSurfaceCapabilitiesKHR = &getSurfaceCapabilities;
    functions.vkGetPhysicalDeviceSurfaceSupportKHR      = &getSurfaceSupport;

    const auto instance = vkwp::Instance::importExternal("external-instance", state.instance);
    const vkwp::PhysicalDevice::OptionalEnumerateArgs optional_args{ .functions = &functions };
    auto physical_devices = vkwp::PhysicalDevice::enumeratePhysicalDevices(instance, optional_args);
    ASSERT_EQ(physical_devices.size(), 1u);
    auto & physical_device = physical_devices[0];
    EXPECT_EQ(static_cast<VkPhysicalDevice>(physical_device), state.physical_device);
    EXPECT_EQ(physical_device.getProperties().deviceID, 77u);

    const auto & queue_families = physical_device.getQueueFamilyProperties();
    ASSERT_EQ(queue_families.size(), 2u);
    EXPECT_EQ(physical_device.listQueueFamilyIndexes(VK_QUEUE_COMPUTE_BIT), std::set<std::uint32_t>({ 0 }));

    const auto & memory_properties = physical_device.getMemoryProperties();
    EXPECT_EQ(memory_properties.memoryTypeCount, 2u);
    EXPECT_EQ(physical_device.getMemoryTypeIndex(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 0b10), std::make_pair(true, 1u));

    const auto extensions = physical_device.enumerateDeviceExtensionProperties();
    ASSERT_EQ(extensions.size(), 1u);
    EXPECT_STREQ(extensions[0].extensionName, "VK_TEST_device");

    const auto format_properties = physical_device.getFormatProperties(VK_FORMAT_R8G8B8A8_UNORM);
    EXPECT_EQ(format_properties.optimalTilingFeatures, VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

    const auto surface = fakeHandle<VkSurfaceKHR>(0x601);
    const auto formats = physical_device.getSurfaceFormats(surface);
    ASSERT_EQ(formats.size(), 1u);
    EXPECT_EQ(formats[0].format, VK_FORMAT_B8G8R8A8_UNORM);
    EXPECT_EQ(physical_device.getSurfaceCapabilities(surface).minImageCount, 2u);
    EXPECT_TRUE(physical_device.isSurfaceSupported(1, surface));
    EXPECT_EQ(state.enumerate_calls, 2);
}
