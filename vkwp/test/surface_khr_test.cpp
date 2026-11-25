// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#if VKWP_SUPPORT_WAYLAND_SURFACE
#define VKWP_USE_WAYLAND_SURFACE

#include "test_utils.hpp"

#include <gtest/gtest.h>
#include <vkwp/instance.hpp>
#include <vkwp/surface_khr.hpp>
#include <vkwp/vulkan_error.hpp>
#include <vkwp/vulkan_functions.hpp>

#include <cstdint>

namespace {

std::int32_t destroy_calls = 0;
VkResult create_result = VK_SUCCESS;

VkResult VKAPI_CALL
createWaylandSurface(const VkInstance instance,
                     const VkWaylandSurfaceCreateInfoKHR * const create_info,
                     const VkAllocationCallbacks * const allocator,
                     VkSurfaceKHR * const surface)
{
    EXPECT_EQ(instance, vkwp::test::fakeHandle<VkInstance>(0x1001));
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_info->sType, VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR);
    EXPECT_EQ(create_info->pNext, vkwp::test::fakePointer(0x1002));
    EXPECT_EQ(create_info->flags, 0u);
    EXPECT_EQ(create_info->display, vkwp::test::fakeHandle<wl_display *>(0x1003));
    EXPECT_EQ(create_info->surface, vkwp::test::fakeHandle<wl_surface *>(0x1004));
    if (create_result == VK_SUCCESS) {
        *surface = vkwp::test::fakeHandle<VkSurfaceKHR>(0x1005);
    }
    return create_result;
}

void VKAPI_CALL
destroySurface(const VkInstance instance,
               const VkSurfaceKHR surface,
               const VkAllocationCallbacks * const allocator)
{
    EXPECT_EQ(instance, vkwp::test::fakeHandle<VkInstance>(0x1001));
    EXPECT_EQ(surface, vkwp::test::fakeHandle<VkSurfaceKHR>(0x1005));
    EXPECT_EQ(allocator, nullptr);
    ++destroy_calls;
}

}  // namespace

TEST(WaylandSurfaceTest, ForwardsCreateInfoAndCachesDestroyFunction)
{
    destroy_calls = 0;
    create_result = VK_SUCCESS;
    vkwp::VulkanFunctions functions;
    functions.vkCreateWaylandSurfaceKHR = createWaylandSurface;
    functions.vkDestroySurfaceKHR = destroySurface;
    const auto instance = vkwp::Instance::importExternal("instance", vkwp::test::fakeHandle<VkInstance>(0x1001));
    {
        const auto surface = vkwp::SurfaceKHR::createWaylandSurfaceKHR(
            "wayland", instance,
            vkwp::test::fakeHandle<wl_display *>(0x1003),
            vkwp::test::fakeHandle<wl_surface *>(0x1004),
            { .p_next = vkwp::test::fakePointer(0x1002), .functions = &functions });
        EXPECT_EQ(static_cast<VkSurfaceKHR>(surface), vkwp::test::fakeHandle<VkSurfaceKHR>(0x1005));
        EXPECT_EQ(surface.getLabel(), "wayland");
        EXPECT_FALSE(surface.isExternal());
        EXPECT_EQ(destroy_calls, 0);
        functions.vkDestroySurfaceKHR = nullptr;
    }
    EXPECT_EQ(destroy_calls, 1);
}

TEST(WaylandSurfaceTest, ThrowsOnCreateFailureWithoutDestroyingSurface)
{
    destroy_calls = 0;
    create_result = VK_ERROR_OUT_OF_HOST_MEMORY;
    vkwp::VulkanFunctions functions;
    functions.vkCreateWaylandSurfaceKHR = createWaylandSurface;
    functions.vkDestroySurfaceKHR = destroySurface;
    const auto instance = vkwp::Instance::importExternal("instance", vkwp::test::fakeHandle<VkInstance>(0x1001));
    EXPECT_THROW(
        (void)vkwp::SurfaceKHR::createWaylandSurfaceKHR(
            "wayland", instance,
            vkwp::test::fakeHandle<wl_display *>(0x1003),
            vkwp::test::fakeHandle<wl_surface *>(0x1004),
            { .p_next = vkwp::test::fakePointer(0x1002), .functions = &functions }),
        vkwp::VulkanError);
    EXPECT_EQ(destroy_calls, 0);
}

#endif
