#include "test_utils.hpp"

#include <gtest/gtest.h>
#include <vkwp/command_buffer.hpp>
#include <vkwp/command_pool.hpp>
#include <vkwp/descriptor_pool.hpp>
#include <vkwp/descriptor_set.hpp>
#include <vkwp/descriptor_set_layout.hpp>
#include <vkwp/device.hpp>
#include <vkwp/device_memory.hpp>
#include <vkwp/instance.hpp>
#include <vkwp/physical_device.hpp>
#include <vkwp/vulkan_functions.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

using vkwp::test::fakeHandle;
using vkwp::test::fakePointer;

struct CommandState
{
    VkDevice                       device       = fakeHandle<VkDevice>(0x2001);
    VkCommandPool                  command_pool = fakeHandle<VkCommandPool>(0x2002);
    std::array<VkCommandBuffer, 2> command_buffers{
        fakeHandle<VkCommandBuffer>(0x2003),
        fakeHandle<VkCommandBuffer>(0x2004),
    };
    int destroy_pool_calls = 0;
    int free_buffer_calls  = 0;
    int begin_calls        = 0;
    int end_calls          = 0;
    int reset_calls        = 0;
};

CommandState command_state;

VkResult VKAPI_CALL
createCommandPool(const VkDevice                  device,
                  const VkCommandPoolCreateInfo * create_info,
                  const VkAllocationCallbacks *   allocator,
                  VkCommandPool *                 command_pool)
{
    EXPECT_EQ(device, command_state.device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_info->pNext, fakePointer(0x2101));
    EXPECT_EQ(create_info->flags, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    EXPECT_EQ(create_info->queueFamilyIndex, 7u);
    *command_pool = command_state.command_pool;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroyCommandPool(const VkDevice device, const VkCommandPool command_pool, const VkAllocationCallbacks * allocator)
{
    ++command_state.destroy_pool_calls;
    EXPECT_EQ(device, command_state.device);
    EXPECT_EQ(command_pool, command_state.command_pool);
    EXPECT_EQ(allocator, nullptr);
}

VkResult VKAPI_CALL
resetCommandPool(const VkDevice device, const VkCommandPool command_pool, const VkCommandPoolResetFlags flags)
{
    ++command_state.reset_calls;
    EXPECT_EQ(device, command_state.device);
    EXPECT_EQ(command_pool, command_state.command_pool);
    EXPECT_EQ(flags, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
    return VK_SUCCESS;
}

VkResult VKAPI_CALL
allocateCommandBuffers(const VkDevice                      device,
                       const VkCommandBufferAllocateInfo * allocate_info,
                       VkCommandBuffer *                   command_buffers)
{
    EXPECT_EQ(device, command_state.device);
    EXPECT_EQ(allocate_info->pNext, fakePointer(0x2102));
    EXPECT_EQ(allocate_info->commandPool, command_state.command_pool);
    EXPECT_EQ(allocate_info->level, VK_COMMAND_BUFFER_LEVEL_SECONDARY);
    EXPECT_EQ(allocate_info->commandBufferCount, 2u);
    command_buffers[0] = command_state.command_buffers[0];
    command_buffers[1] = command_state.command_buffers[1];
    return VK_SUCCESS;
}

void VKAPI_CALL
freeCommandBuffers(const VkDevice          device,
                   const VkCommandPool     command_pool,
                   const std::uint32_t     command_buffer_count,
                   const VkCommandBuffer * command_buffers)
{
    ++command_state.free_buffer_calls;
    EXPECT_EQ(device, command_state.device);
    EXPECT_EQ(command_pool, command_state.command_pool);
    EXPECT_EQ(command_buffer_count, 1u);
    EXPECT_TRUE(command_buffers[0] == command_state.command_buffers[0]
                || command_buffers[0] == command_state.command_buffers[1]);
}

VkResult VKAPI_CALL
beginCommandBuffer(const VkCommandBuffer command_buffer, const VkCommandBufferBeginInfo * begin_info)
{
    ++command_state.begin_calls;
    EXPECT_EQ(command_buffer, command_state.command_buffers[0]);
    EXPECT_EQ(begin_info->pNext, fakePointer(0x2103));
    EXPECT_EQ(begin_info->flags, VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT);
    EXPECT_NE(begin_info->pInheritanceInfo, nullptr);
    EXPECT_EQ(begin_info->pInheritanceInfo->subpass, 3u);
    return VK_SUCCESS;
}

VkResult VKAPI_CALL
endCommandBuffer(const VkCommandBuffer command_buffer)
{
    ++command_state.end_calls;
    EXPECT_EQ(command_buffer, command_state.command_buffers[0]);
    return VK_SUCCESS;
}

struct DescriptorState
{
    VkDevice                             device = fakeHandle<VkDevice>(0x2201);
    VkDescriptorPool                     pool   = fakeHandle<VkDescriptorPool>(0x2202);
    std::array<VkDescriptorSetLayout, 2> layouts{
        fakeHandle<VkDescriptorSetLayout>(0x2203),
        fakeHandle<VkDescriptorSetLayout>(0x2204),
    };
    std::array<VkDescriptorSet, 2> sets{
        fakeHandle<VkDescriptorSet>(0x2205),
        fakeHandle<VkDescriptorSet>(0x2206),
    };
    int destroy_pool_calls   = 0;
    int destroy_layout_calls = 0;
    int free_set_calls       = 0;
    int next_layout          = 0;
};

DescriptorState descriptor_state;

VkResult VKAPI_CALL
createDescriptorPool(const VkDevice                     device,
                     const VkDescriptorPoolCreateInfo * create_info,
                     const VkAllocationCallbacks *      allocator,
                     VkDescriptorPool *                 pool)
{
    EXPECT_EQ(device, descriptor_state.device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_info->flags, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
    EXPECT_EQ(create_info->maxSets, 4u);
    EXPECT_EQ(create_info->poolSizeCount, 1u);
    EXPECT_EQ(create_info->pPoolSizes[0].type, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    EXPECT_EQ(create_info->pPoolSizes[0].descriptorCount, 4u);
    *pool = descriptor_state.pool;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroyDescriptorPool(const VkDevice device, const VkDescriptorPool pool, const VkAllocationCallbacks * allocator)
{
    ++descriptor_state.destroy_pool_calls;
    EXPECT_EQ(device, descriptor_state.device);
    EXPECT_EQ(pool, descriptor_state.pool);
    EXPECT_EQ(allocator, nullptr);
}

VkResult VKAPI_CALL
createDescriptorSetLayout(const VkDevice                          device,
                          const VkDescriptorSetLayoutCreateInfo * create_info,
                          const VkAllocationCallbacks *           allocator,
                          VkDescriptorSetLayout *                 layout)
{
    EXPECT_EQ(device, descriptor_state.device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_info->bindingCount, 1u);
    EXPECT_EQ(create_info->pBindings[0].binding, static_cast<std::uint32_t>(descriptor_state.next_layout));
    EXPECT_EQ(create_info->pBindings[0].descriptorType, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    *layout = descriptor_state.layouts[static_cast<std::size_t>(descriptor_state.next_layout)];
    ++descriptor_state.next_layout;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroyDescriptorSetLayout(const VkDevice                device,
                           const VkDescriptorSetLayout   layout,
                           const VkAllocationCallbacks * allocator)
{
    ++descriptor_state.destroy_layout_calls;
    EXPECT_EQ(device, descriptor_state.device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_TRUE(layout == descriptor_state.layouts[0] || layout == descriptor_state.layouts[1]);
}

VkResult VKAPI_CALL
allocateDescriptorSets(const VkDevice device, const VkDescriptorSetAllocateInfo * allocate_info, VkDescriptorSet * sets)
{
    EXPECT_EQ(device, descriptor_state.device);
    EXPECT_EQ(allocate_info->pNext, fakePointer(0x2207));
    EXPECT_EQ(allocate_info->descriptorPool, descriptor_state.pool);
    EXPECT_EQ(allocate_info->descriptorSetCount, 2u);
    EXPECT_EQ(allocate_info->pSetLayouts[0], descriptor_state.layouts[0]);
    EXPECT_EQ(allocate_info->pSetLayouts[1], descriptor_state.layouts[1]);
    sets[0] = descriptor_state.sets[0];
    sets[1] = descriptor_state.sets[1];
    return VK_SUCCESS;
}

VkResult VKAPI_CALL
freeDescriptorSets(const VkDevice          device,
                   const VkDescriptorPool  pool,
                   const std::uint32_t     descriptor_set_count,
                   const VkDescriptorSet * sets)
{
    ++descriptor_state.free_set_calls;
    EXPECT_EQ(device, descriptor_state.device);
    EXPECT_EQ(pool, descriptor_state.pool);
    EXPECT_EQ(descriptor_set_count, 1u);
    EXPECT_TRUE(sets[0] == descriptor_state.sets[0] || sets[0] == descriptor_state.sets[1]);
    return VK_SUCCESS;
}

struct MemoryState
{
    VkInstance                instance        = fakeHandle<VkInstance>(0x2301);
    VkPhysicalDevice          physical_device = fakeHandle<VkPhysicalDevice>(0x2302);
    VkDevice                  device          = fakeHandle<VkDevice>(0x2303);
    VkDeviceMemory            memory          = fakeHandle<VkDeviceMemory>(0x2304);
    std::array<std::byte, 32> bytes{};
    int                       allocate_calls = 0;
    int                       free_calls     = 0;
    int                       map_calls      = 0;
    int                       unmap_calls    = 0;
};

MemoryState memory_state;

VkResult VKAPI_CALL
enumeratePhysicalDevicesForMemory(const VkInstance instance, std::uint32_t * count, VkPhysicalDevice * physical_devices)
{
    EXPECT_EQ(instance, memory_state.instance);
    if (physical_devices == nullptr)
    {
        *count = 1;
    }
    else
    {
        physical_devices[0] = memory_state.physical_device;
    }
    return VK_SUCCESS;
}

void VKAPI_CALL
getPhysicalDevicePropertiesForMemory(const VkPhysicalDevice physical_device, VkPhysicalDeviceProperties * properties)
{
    EXPECT_EQ(physical_device, memory_state.physical_device);
    *properties = {};
    std::memcpy(properties->deviceName, "Memory GPU", sizeof("Memory GPU"));
}

void VKAPI_CALL
getPhysicalDeviceMemoryPropertiesForMemory(const VkPhysicalDevice             physical_device,
                                           VkPhysicalDeviceMemoryProperties * properties)
{
    EXPECT_EQ(physical_device, memory_state.physical_device);
    *properties                              = {};
    properties->memoryTypeCount              = 2;
    properties->memoryTypes[0].propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    properties->memoryTypes[1].propertyFlags
        = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
}

VkResult VKAPI_CALL
allocateMemory(const VkDevice                device,
               const VkMemoryAllocateInfo *  allocate_info,
               const VkAllocationCallbacks * allocator,
               VkDeviceMemory *              memory)
{
    ++memory_state.allocate_calls;
    EXPECT_EQ(device, memory_state.device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(allocate_info->pNext, fakePointer(0x2305));
    EXPECT_EQ(allocate_info->allocationSize, 32u);
    EXPECT_EQ(allocate_info->memoryTypeIndex, 1u);
    *memory = memory_state.memory;
    return VK_SUCCESS;
}

void VKAPI_CALL
freeMemory(const VkDevice device, const VkDeviceMemory memory, const VkAllocationCallbacks * allocator)
{
    ++memory_state.free_calls;
    EXPECT_EQ(device, memory_state.device);
    EXPECT_EQ(memory, memory_state.memory);
    EXPECT_EQ(allocator, nullptr);
}

VkResult VKAPI_CALL
mapMemory(const VkDevice         device,
          const VkDeviceMemory   memory,
          const VkDeviceSize     offset,
          const VkDeviceSize     size,
          const VkMemoryMapFlags flags,
          void **                data)
{
    ++memory_state.map_calls;
    EXPECT_EQ(device, memory_state.device);
    EXPECT_EQ(memory, memory_state.memory);
    EXPECT_EQ(flags, 0u);
    EXPECT_LE(offset + size, memory_state.bytes.size());
    *data = memory_state.bytes.data() + static_cast<std::size_t>(offset);
    return VK_SUCCESS;
}

void VKAPI_CALL
unmapMemory(const VkDevice device, const VkDeviceMemory memory)
{
    ++memory_state.unmap_calls;
    EXPECT_EQ(device, memory_state.device);
    EXPECT_EQ(memory, memory_state.memory);
}

}  // namespace

TEST(VulkanFunctionsCommandTest, CommandPoolAndBuffersForwardArgumentsAndFree)
{
    command_state = {};
    vkwp::VulkanFunctions functions;
    functions.vkCreateCommandPool      = &createCommandPool;
    functions.vkDestroyCommandPool     = &destroyCommandPool;
    functions.vkResetCommandPool       = &resetCommandPool;
    functions.vkAllocateCommandBuffers = &allocateCommandBuffers;
    functions.vkFreeCommandBuffers     = &freeCommandBuffers;
    functions.vkBeginCommandBuffer     = &beginCommandBuffer;
    functions.vkEndCommandBuffer       = &endCommandBuffer;
    const vkwp::Device device{ "device", command_state.device };

    {
        const vkwp::CommandPool::OptionalCreateArgs pool_args{
            .flags       = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .p_next      = fakePointer(0x2101),
            .debug_utils = nullptr,
            .functions   = &functions,
        };
        auto pool = vkwp::CommandPool::create("pool", device, 7, nullptr, pool_args);
        pool.resetCommandPool(VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
        {
            const vkwp::CommandBuffer::OptionalAllocateArgs allocate_args{
                .p_next      = fakePointer(0x2102),
                .debug_utils = nullptr,
                .functions   = &functions,
            };
            auto command_buffers = vkwp::CommandBuffer::allocateCommandBuffers(pool,
                                                                               VK_COMMAND_BUFFER_LEVEL_SECONDARY,
                                                                               { "first", "second" },
                                                                               allocate_args);
            ASSERT_EQ(command_buffers.size(), 2u);
            const VkCommandBufferInheritanceInfo inheritance_info{
                .sType       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
                .pNext       = nullptr,
                .renderPass  = nullptr,
                .subpass     = 3,
                .framebuffer = nullptr,
            };
            command_buffers[0].beginCommandBuffer(inheritance_info,
                                                  VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
                                                  fakePointer(0x2103));
            command_buffers[0].endCommandBuffer();
        }
        EXPECT_EQ(command_state.free_buffer_calls, 2);
        EXPECT_EQ(command_state.destroy_pool_calls, 0);
    }
    EXPECT_EQ(command_state.reset_calls, 1);
    EXPECT_EQ(command_state.begin_calls, 1);
    EXPECT_EQ(command_state.end_calls, 1);
    EXPECT_EQ(command_state.destroy_pool_calls, 1);
}

TEST(VulkanFunctionsDescriptorTest, DescriptorSetsForwardLayoutsAndFreeBeforePoolDestruction)
{
    descriptor_state = {};
    vkwp::VulkanFunctions functions;
    functions.vkCreateDescriptorPool       = &createDescriptorPool;
    functions.vkDestroyDescriptorPool      = &destroyDescriptorPool;
    functions.vkCreateDescriptorSetLayout  = &createDescriptorSetLayout;
    functions.vkDestroyDescriptorSetLayout = &destroyDescriptorSetLayout;
    functions.vkAllocateDescriptorSets     = &allocateDescriptorSets;
    functions.vkFreeDescriptorSets         = &freeDescriptorSets;
    const vkwp::Device device{ "device", descriptor_state.device };

    {
        const VkDescriptorSetLayoutBinding binding0{
            .binding         = 0,
            .descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags      = VK_SHADER_STAGE_COMPUTE_BIT,
        };
        const VkDescriptorSetLayoutBinding binding1{
            .binding         = 1,
            .descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags      = VK_SHADER_STAGE_COMPUTE_BIT,
        };
        const vkwp::DescriptorSetLayout::OptionalCreateArgs layout_args{
            .functions = &functions,
        };
        auto layout0 = vkwp::DescriptorSetLayout::create("layout-0", device, { binding0 }, layout_args);
        auto layout1 = vkwp::DescriptorSetLayout::create("layout-1", device, { binding1 }, layout_args);

        const VkDescriptorPoolSize pool_size{
            .type            = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 4,
        };
        const vkwp::DescriptorPool::OptionalCreateArgs pool_args{
            .flags     = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
            .functions = &functions,
        };
        auto pool = vkwp::DescriptorPool::create("pool", device, 4, { pool_size }, pool_args);
        {
            const std::vector<vkwp::DescriptorSetAllocateInfo> allocate_infos{
                { .label = "set-0", .descriptor_set_layout = layout0 },
                { .label = "set-1", .descriptor_set_layout = layout1 },
            };
            const vkwp::DescriptorSet::OptionalAllocateArgs allocate_args{
                .p_next      = fakePointer(0x2207),
                .debug_utils = nullptr,
                .functions   = &functions,
            };
            const auto sets = vkwp::DescriptorSet::allocate(pool, allocate_infos, allocate_args);
            ASSERT_EQ(sets.size(), 2u);
            EXPECT_EQ(static_cast<VkDescriptorSet>(sets[0]), descriptor_state.sets[0]);
            EXPECT_EQ(static_cast<VkDescriptorSet>(sets[1]), descriptor_state.sets[1]);
        }
        EXPECT_EQ(descriptor_state.free_set_calls, 2);
    }
    EXPECT_EQ(descriptor_state.destroy_pool_calls, 1);
    EXPECT_EQ(descriptor_state.destroy_layout_calls, 2);
}

TEST(VulkanFunctionsMemoryTest, AllocateMapReadWriteUnmapAndFreeUseInjectedFunctions)
{
    memory_state = {};
    vkwp::VulkanFunctions functions;
    functions.vkEnumeratePhysicalDevices          = &enumeratePhysicalDevicesForMemory;
    functions.vkGetPhysicalDeviceProperties       = &getPhysicalDevicePropertiesForMemory;
    functions.vkGetPhysicalDeviceMemoryProperties = &getPhysicalDeviceMemoryPropertiesForMemory;
    functions.vkAllocateMemory                    = &allocateMemory;
    functions.vkFreeMemory                        = &freeMemory;
    functions.vkMapMemory                         = &mapMemory;
    functions.vkUnmapMemory                       = &unmapMemory;

    const vkwp::Instance                              instance{ "instance", memory_state.instance };
    const vkwp::PhysicalDevice::OptionalEnumerateArgs enumerate_args{ .functions = &functions };
    auto physical_devices = vkwp::PhysicalDevice::enumeratePhysicalDevices(instance, enumerate_args);
    ASSERT_EQ(physical_devices.size(), 1u);
    const vkwp::Device device{ "device", memory_state.device };
    {
        const VkMemoryRequirements requirements{
            .size           = 32,
            .alignment      = 8,
            .memoryTypeBits = 0b10,
        };
        const vkwp::DeviceMemory::OptionalAllocateArgs allocate_args{
            .p_next      = fakePointer(0x2305),
            .debug_utils = nullptr,
            .functions   = &functions,
        };
        auto                           memory = vkwp::DeviceMemory::allocate("memory",
                                                   device,
                                                   physical_devices[0],
                                                   requirements,
                                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                                   allocate_args);
        const std::array<std::byte, 4> source{ std::byte{ 0x11 },
                                               std::byte{ 0x22 },
                                               std::byte{ 0x33 },
                                               std::byte{ 0x44 } };
        memory.write(source.data(), source.size(), 5);
        std::array<std::byte, 4> destination{};
        memory.read(destination.data(), destination.size(), 5);
        EXPECT_EQ(destination, source);
        {
            auto mapped = memory.map(8, 2);
            EXPECT_TRUE(mapped.isMapped());
            EXPECT_EQ(mapped.size(), 8u);
            EXPECT_EQ(mapped.ptr(), memory_state.bytes.data() + 2);
        }
    }
    EXPECT_EQ(memory_state.allocate_calls, 1);
    EXPECT_EQ(memory_state.map_calls, 3);
    EXPECT_EQ(memory_state.unmap_calls, 3);
    EXPECT_EQ(memory_state.free_calls, 1);
}
