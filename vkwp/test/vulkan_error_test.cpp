// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#include "test_utils.hpp"

#include <gtest/gtest.h>
#include <vkwp/buffer.hpp>
#include <vkwp/device.hpp>
#include <vkwp/device_memory.hpp>
#include <vkwp/fence.hpp>
#include <vkwp/vulkan_error.hpp>
#include <vkwp/vulkan_functions.hpp>

#include <array>
#include <string>
#include <string_view>

namespace {

using vkwp::test::fakeHandle;

struct ErrorState
{
    VkDevice       device        = fakeHandle<VkDevice>(0x4001);
    VkBuffer       buffer        = fakeHandle<VkBuffer>(0x4002);
    VkDeviceMemory memory        = fakeHandle<VkDeviceMemory>(0x4003);
    VkFence        fence         = fakeHandle<VkFence>(0x4004);
    int            create_calls  = 0;
    int            destroy_calls = 0;
};

ErrorState error_state;

VkResult VKAPI_CALL
failCreateBuffer(const VkDevice                device,
                 const VkBufferCreateInfo *    create_info,
                 const VkAllocationCallbacks * allocator,
                 VkBuffer *                    buffer)
{
    ++error_state.create_calls;
    EXPECT_EQ(device, error_state.device);
    EXPECT_EQ(create_info->size, 64u);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_NE(buffer, nullptr);
    return VK_ERROR_OUT_OF_DEVICE_MEMORY;
}

VkResult VKAPI_CALL
createBufferForOperationError(const VkDevice device,
                              const VkBufferCreateInfo *,
                              const VkAllocationCallbacks *,
                              VkBuffer * buffer)
{
    ++error_state.create_calls;
    EXPECT_EQ(device, error_state.device);
    *buffer = error_state.buffer;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroyBuffer(const VkDevice device, const VkBuffer buffer, const VkAllocationCallbacks * allocator)
{
    ++error_state.destroy_calls;
    EXPECT_EQ(device, error_state.device);
    EXPECT_EQ(buffer, error_state.buffer);
    EXPECT_EQ(allocator, nullptr);
}

VkResult VKAPI_CALL
failBindBufferMemory(const VkDevice       device,
                     const VkBuffer       buffer,
                     const VkDeviceMemory memory,
                     const VkDeviceSize   offset)
{
    EXPECT_EQ(device, error_state.device);
    EXPECT_EQ(buffer, error_state.buffer);
    EXPECT_EQ(memory, error_state.memory);
    EXPECT_EQ(offset, 16u);
    return VK_ERROR_MEMORY_MAP_FAILED;
}

VkResult VKAPI_CALL
createFence(const VkDevice device, const VkFenceCreateInfo *, const VkAllocationCallbacks *, VkFence * fence)
{
    EXPECT_EQ(device, error_state.device);
    *fence = error_state.fence;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroyFence(const VkDevice device, const VkFence fence, const VkAllocationCallbacks *)
{
    ++error_state.destroy_calls;
    EXPECT_EQ(device, error_state.device);
    EXPECT_EQ(fence, error_state.fence);
}

VkResult VKAPI_CALL
waitForFenceTimeout(const VkDevice, const std::uint32_t, const VkFence *, const VkBool32, const std::uint64_t)
{
    return VK_TIMEOUT;
}

VkResult VKAPI_CALL
waitForFenceDeviceLost(const VkDevice, const std::uint32_t, const VkFence *, const VkBool32, const std::uint64_t)
{
    return VK_ERROR_DEVICE_LOST;
}

VkResult VKAPI_CALL
resetFences(const VkDevice, const std::uint32_t, const VkFence *)
{
    return VK_SUCCESS;
}

}  // namespace

TEST(VulkanErrorTest, SuccessDoesNotThrow)
{
    EXPECT_NO_THROW(vkwp::VulkanError::check(VK_SUCCESS, "successful call"));
}

TEST(VulkanErrorTest, EveryNonSuccessCategoryThrowsAndPreservesResultAndMessage)
{
    struct Case
    {
        VkResult         result;
        std::string_view result_name;
    };
    constexpr std::array cases{
        Case{ VK_NOT_READY, "VK_NOT_READY" },
        Case{ VK_TIMEOUT, "VK_TIMEOUT" },
        Case{ VK_INCOMPLETE, "VK_INCOMPLETE" },
        Case{ VK_SUBOPTIMAL_KHR, "VK_SUBOPTIMAL_KHR" },
        Case{ VK_ERROR_OUT_OF_HOST_MEMORY, "VK_ERROR_OUT_OF_HOST_MEMORY" },
        Case{ VK_ERROR_DEVICE_LOST, "VK_ERROR_DEVICE_LOST" },
        Case{ VK_ERROR_OUT_OF_DATE_KHR, "VK_ERROR_OUT_OF_DATE_KHR" },
    };

    for (const auto & test_case : cases)
    {
        SCOPED_TRACE(test_case.result_name);
        try
        {
            vkwp::VulkanError::check(test_case.result, "mock operation failed");
            FAIL() << "VulkanError was not thrown";
        }
        catch (const vkwp::VulkanError & error)
        {
            EXPECT_EQ(error.result(), test_case.result);
            const std::string message{ error.what() };
            EXPECT_NE(message.find("mock operation failed"), std::string::npos);
            EXPECT_NE(message.find(test_case.result_name), std::string::npos);
        }
    }
}

TEST(VulkanErrorTest, UnknownResultCodeIsReportedNumerically)
{
    constexpr auto unknown_result = static_cast<VkResult>(-1234567);
    try
    {
        vkwp::VulkanError::check(unknown_result, "unknown failure");
        FAIL() << "VulkanError was not thrown";
    }
    catch (const vkwp::VulkanError & error)
    {
        EXPECT_EQ(error.result(), unknown_result);
        const std::string message{ error.what() };
        EXPECT_NE(message.find("unknown failure"), std::string::npos);
        EXPECT_NE(message.find("Unknown result code -1234567"), std::string::npos);
    }
}

TEST(VulkanErrorIntegrationTest, CreateErrorPropagatesWithoutDestroyingNullHandle)
{
    error_state = {};
    vkwp::VulkanFunctions functions;
    functions.vkCreateBuffer  = &failCreateBuffer;
    functions.vkDestroyBuffer = &destroyBuffer;
    const auto device         = vkwp::Device::importExternal("device", error_state.device, &functions);
    const vkwp::Buffer::OptionalCreateArgs optional_args{
        .functions = &functions,
    };

    try
    {
        static_cast<void>(vkwp::Buffer::create("buffer", device, 64, VK_BUFFER_USAGE_TRANSFER_DST_BIT, optional_args));
        FAIL() << "VulkanError was not thrown";
    }
    catch (const vkwp::VulkanError & error)
    {
        EXPECT_EQ(error.result(), VK_ERROR_OUT_OF_DEVICE_MEMORY);
        EXPECT_NE(std::string{ error.what() }.find("vkCreateBuffer"), std::string::npos);
    }
    EXPECT_EQ(error_state.create_calls, 1);
    EXPECT_EQ(error_state.destroy_calls, 0);
}

TEST(VulkanErrorIntegrationTest, MemberFunctionErrorPropagatesAndResourceStillDestroys)
{
    error_state = {};
    vkwp::VulkanFunctions functions;
    functions.vkCreateBuffer                = &createBufferForOperationError;
    functions.vkDestroyBuffer               = &destroyBuffer;
    functions.vkBindBufferMemory            = &failBindBufferMemory;
    functions.vkGetBufferMemoryRequirements = nullptr;
    functions.vkGetBufferDeviceAddress      = nullptr;
    const auto device                       = vkwp::Device::importExternal("device", error_state.device, &functions);
    const auto memory = vkwp::DeviceMemory::importExternal("memory", device, error_state.memory, &functions);

    {
        const vkwp::Buffer::OptionalCreateArgs optional_args{
            .functions = &functions,
        };
        auto buffer = vkwp::Buffer::create("buffer", device, 64, VK_BUFFER_USAGE_TRANSFER_DST_BIT, optional_args);
        try
        {
            buffer.bindMemory(memory, 16);
            FAIL() << "VulkanError was not thrown";
        }
        catch (const vkwp::VulkanError & error)
        {
            EXPECT_EQ(error.result(), VK_ERROR_MEMORY_MAP_FAILED);
            EXPECT_NE(std::string{ error.what() }.find("vkBindBufferMemory"), std::string::npos);
        }
        EXPECT_EQ(error_state.destroy_calls, 0);
    }
    EXPECT_EQ(error_state.destroy_calls, 1);
}

TEST(VulkanErrorIntegrationTest, FenceHandlesTimeoutButThrowsOtherFailures)
{
    error_state = {};
    vkwp::VulkanFunctions functions;
    functions.vkCreateFence   = &createFence;
    functions.vkDestroyFence  = &destroyFence;
    functions.vkWaitForFences = &waitForFenceTimeout;
    functions.vkResetFences   = &resetFences;
    const auto device         = vkwp::Device::importExternal("device", error_state.device, &functions);
    const vkwp::Fence::OptionalCreateArgs optional_args{
        .functions = &functions,
    };
    auto fence = vkwp::Fence::create("fence", device, optional_args);
    EXPECT_FALSE(fence.wait(100));

    functions.vkWaitForFences = &waitForFenceDeviceLost;
    const auto failing_fence  = vkwp::Fence::create("failing-fence", device, optional_args);
    try
    {
        static_cast<void>(failing_fence.wait(100));
        FAIL() << "VulkanError was not thrown";
    }
    catch (const vkwp::VulkanError & error)
    {
        EXPECT_EQ(error.result(), VK_ERROR_DEVICE_LOST);
    }
}
