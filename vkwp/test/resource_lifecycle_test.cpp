#include "test_utils.hpp"

#include <gtest/gtest.h>
#include <vkwp/buffer.hpp>
#include <vkwp/descriptor_pool.hpp>
#include <vkwp/descriptor_set_layout.hpp>
#include <vkwp/device.hpp>
#include <vkwp/device_memory.hpp>
#include <vkwp/fence.hpp>
#include <vkwp/framebuffer.hpp>
#include <vkwp/image.hpp>
#include <vkwp/image_view.hpp>
#include <vkwp/render_pass.hpp>
#include <vkwp/sampler.hpp>
#include <vkwp/semaphore.hpp>
#include <vkwp/shader_module.hpp>
#include <vkwp/vulkan_functions.hpp>

#include <array>
#include <cstdint>
#include <functional>
#include <vector>

namespace {

using vkwp::test::fakeHandle;
using vkwp::test::fakePointer;

struct ResourceState
{
    VkDevice              device                = fakeHandle<VkDevice>(0x1001);
    VkBuffer              buffer                = fakeHandle<VkBuffer>(0x1002);
    VkDeviceMemory        memory                = fakeHandle<VkDeviceMemory>(0x1003);
    VkImage               image                 = fakeHandle<VkImage>(0x1004);
    VkImageView           image_view            = fakeHandle<VkImageView>(0x1005);
    VkRenderPass          render_pass           = fakeHandle<VkRenderPass>(0x1006);
    VkFramebuffer         framebuffer           = fakeHandle<VkFramebuffer>(0x1007);
    VkFence               fence                 = fakeHandle<VkFence>(0x1008);
    VkSampler             sampler               = fakeHandle<VkSampler>(0x1009);
    VkSemaphore           semaphore             = fakeHandle<VkSemaphore>(0x1010);
    VkShaderModule        shader_module         = fakeHandle<VkShaderModule>(0x1011);
    VkDescriptorPool      descriptor_pool       = fakeHandle<VkDescriptorPool>(0x1012);
    VkDescriptorSetLayout descriptor_set_layout = fakeHandle<VkDescriptorSetLayout>(0x1013);
    int                   create_calls          = 0;
    int                   destroy_calls         = 0;
    int                   operation_calls       = 0;
};

ResourceState state;

VkResult VKAPI_CALL
createBuffer(const VkDevice                device,
             const VkBufferCreateInfo *    create_info,
             const VkAllocationCallbacks * allocator,
             VkBuffer *                    buffer)
{
    ++state.create_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_info->pNext, fakePointer(0x1101));
    EXPECT_EQ(create_info->flags, VK_BUFFER_CREATE_SPARSE_BINDING_BIT);
    EXPECT_EQ(create_info->size, 4096u);
    EXPECT_EQ(create_info->usage, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
    EXPECT_EQ(create_info->sharingMode, VK_SHARING_MODE_EXCLUSIVE);
    *buffer = state.buffer;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroyBuffer(const VkDevice device, const VkBuffer buffer, const VkAllocationCallbacks * allocator)
{
    ++state.destroy_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(buffer, state.buffer);
    EXPECT_EQ(allocator, nullptr);
}

void VKAPI_CALL
getBufferMemoryRequirements(const VkDevice device, const VkBuffer buffer, VkMemoryRequirements * requirements)
{
    ++state.operation_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(buffer, state.buffer);
    *requirements = { .size = 8192, .alignment = 256, .memoryTypeBits = 0x15 };
}

VkResult VKAPI_CALL
bindBufferMemory(const VkDevice device, const VkBuffer buffer, const VkDeviceMemory memory, const VkDeviceSize offset)
{
    ++state.operation_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(buffer, state.buffer);
    EXPECT_EQ(memory, state.memory);
    EXPECT_EQ(offset, 512u);
    return VK_SUCCESS;
}

VkDeviceAddress VKAPI_CALL
getBufferDeviceAddress(const VkDevice device, const VkBufferDeviceAddressInfo * info)
{
    ++state.operation_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(info->sType, VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO);
    EXPECT_EQ(info->buffer, state.buffer);
    return 0x12345678u;
}

VkResult VKAPI_CALL
createImage(const VkDevice                device,
            const VkImageCreateInfo *     create_info,
            const VkAllocationCallbacks * allocator,
            VkImage *                     image)
{
    ++state.create_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_info->pNext, fakePointer(0x1102));
    EXPECT_EQ(create_info->flags, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
    EXPECT_EQ(create_info->imageType, VK_IMAGE_TYPE_2D);
    EXPECT_EQ(create_info->format, VK_FORMAT_R8G8B8A8_UNORM);
    EXPECT_EQ(create_info->extent.width, 640u);
    EXPECT_EQ(create_info->extent.height, 480u);
    EXPECT_EQ(create_info->extent.depth, 1u);
    EXPECT_EQ(create_info->mipLevels, 4u);
    EXPECT_EQ(create_info->arrayLayers, 6u);
    EXPECT_EQ(create_info->samples, VK_SAMPLE_COUNT_4_BIT);
    EXPECT_EQ(create_info->tiling, VK_IMAGE_TILING_LINEAR);
    EXPECT_EQ(create_info->usage, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    *image = state.image;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroyImage(const VkDevice device, const VkImage image, const VkAllocationCallbacks * allocator)
{
    ++state.destroy_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(image, state.image);
    EXPECT_EQ(allocator, nullptr);
}

void VKAPI_CALL
getImageMemoryRequirements(const VkDevice device, const VkImage image, VkMemoryRequirements * requirements)
{
    ++state.operation_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(image, state.image);
    *requirements = { .size = 16384, .alignment = 512, .memoryTypeBits = 0x3 };
}

void VKAPI_CALL
getImageMemoryRequirements2(const VkDevice                         device,
                            const VkImageMemoryRequirementsInfo2 * info,
                            VkMemoryRequirements2 *                requirements)
{
    ++state.operation_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(info->pNext, fakePointer(0x1103));
    EXPECT_EQ(info->image, state.image);
    *requirements = {
        .sType              = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
        .pNext              = nullptr,
        .memoryRequirements = { .size = 32768, .alignment = 1024, .memoryTypeBits = 0x7 },
    };
}

VkResult VKAPI_CALL
bindImageMemory(const VkDevice device, const VkImage image, const VkDeviceMemory memory, const VkDeviceSize offset)
{
    ++state.operation_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(image, state.image);
    EXPECT_EQ(memory, state.memory);
    EXPECT_EQ(offset, 2048u);
    return VK_SUCCESS;
}

VkResult VKAPI_CALL
createImageView(const VkDevice                device,
                const VkImageViewCreateInfo * create_info,
                const VkAllocationCallbacks * allocator,
                VkImageView *                 image_view)
{
    ++state.create_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_info->image, state.image);
    EXPECT_EQ(create_info->viewType, VK_IMAGE_VIEW_TYPE_2D_ARRAY);
    EXPECT_EQ(create_info->format, VK_FORMAT_R8G8B8A8_UNORM);
    EXPECT_EQ(create_info->components.r, VK_COMPONENT_SWIZZLE_B);
    EXPECT_EQ(create_info->subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    EXPECT_EQ(create_info->subresourceRange.levelCount, 4u);
    EXPECT_EQ(create_info->subresourceRange.layerCount, 6u);
    *image_view = state.image_view;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroyImageView(const VkDevice device, const VkImageView image_view, const VkAllocationCallbacks * allocator)
{
    ++state.destroy_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(image_view, state.image_view);
    EXPECT_EQ(allocator, nullptr);
}

VkResult VKAPI_CALL
createRenderPass(const VkDevice                 device,
                 const VkRenderPassCreateInfo * create_info,
                 const VkAllocationCallbacks *  allocator,
                 VkRenderPass *                 render_pass)
{
    ++state.create_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_info->attachmentCount, 1u);
    EXPECT_EQ(create_info->pAttachments[0].format, VK_FORMAT_R8G8B8A8_UNORM);
    EXPECT_EQ(create_info->subpassCount, 1u);
    EXPECT_EQ(create_info->dependencyCount, 1u);
    *render_pass = state.render_pass;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroyRenderPass(const VkDevice device, const VkRenderPass render_pass, const VkAllocationCallbacks * allocator)
{
    ++state.destroy_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(render_pass, state.render_pass);
    EXPECT_EQ(allocator, nullptr);
}

VkResult VKAPI_CALL
createFramebuffer(const VkDevice                  device,
                  const VkFramebufferCreateInfo * create_info,
                  const VkAllocationCallbacks *   allocator,
                  VkFramebuffer *                 framebuffer)
{
    ++state.create_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_info->renderPass, state.render_pass);
    EXPECT_EQ(create_info->attachmentCount, 1u);
    EXPECT_EQ(create_info->pAttachments[0], state.image_view);
    EXPECT_EQ(create_info->width, 640u);
    EXPECT_EQ(create_info->height, 480u);
    EXPECT_EQ(create_info->layers, 2u);
    *framebuffer = state.framebuffer;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroyFramebuffer(const VkDevice device, const VkFramebuffer framebuffer, const VkAllocationCallbacks * allocator)
{
    ++state.destroy_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(framebuffer, state.framebuffer);
    EXPECT_EQ(allocator, nullptr);
}

VkResult VKAPI_CALL
createFence(const VkDevice                device,
            const VkFenceCreateInfo *     create_info,
            const VkAllocationCallbacks * allocator,
            VkFence *                     fence)
{
    ++state.create_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_info->pNext, fakePointer(0x1104));
    EXPECT_EQ(create_info->flags, VK_FENCE_CREATE_SIGNALED_BIT);
    *fence = state.fence;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroyFence(const VkDevice device, const VkFence fence, const VkAllocationCallbacks * allocator)
{
    ++state.destroy_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(fence, state.fence);
    EXPECT_EQ(allocator, nullptr);
}

VkResult VKAPI_CALL
waitForFences(const VkDevice      device,
              const std::uint32_t fence_count,
              const VkFence *     fences,
              const VkBool32      wait_all,
              const std::uint64_t timeout)
{
    ++state.operation_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(fence_count, 1u);
    EXPECT_EQ(fences[0], state.fence);
    EXPECT_EQ(wait_all, VK_TRUE);
    EXPECT_EQ(timeout, 12345u);
    return VK_SUCCESS;
}

VkResult VKAPI_CALL
resetFences(const VkDevice device, const std::uint32_t fence_count, const VkFence * fences)
{
    ++state.operation_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(fence_count, 1u);
    EXPECT_EQ(fences[0], state.fence);
    return VK_SUCCESS;
}

VkResult VKAPI_CALL
createSampler(const VkDevice                device,
              const VkSamplerCreateInfo *   create_info,
              const VkAllocationCallbacks * allocator,
              VkSampler *                   sampler)
{
    ++state.create_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_info->magFilter, VK_FILTER_LINEAR);
    EXPECT_EQ(create_info->minFilter, VK_FILTER_NEAREST);
    EXPECT_EQ(create_info->mipmapMode, VK_SAMPLER_MIPMAP_MODE_LINEAR);
    EXPECT_EQ(create_info->addressModeU, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    EXPECT_EQ(create_info->addressModeV, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
    EXPECT_EQ(create_info->addressModeW, VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT);
    EXPECT_EQ(create_info->anisotropyEnable, VK_TRUE);
    EXPECT_FLOAT_EQ(create_info->maxAnisotropy, 8.0f);
    *sampler = state.sampler;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroySampler(const VkDevice device, const VkSampler sampler, const VkAllocationCallbacks * allocator)
{
    ++state.destroy_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(sampler, state.sampler);
    EXPECT_EQ(allocator, nullptr);
}

VkResult VKAPI_CALL
createSemaphore(const VkDevice                device,
                const VkSemaphoreCreateInfo * create_info,
                const VkAllocationCallbacks * allocator,
                VkSemaphore *                 semaphore)
{
    ++state.create_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_info->pNext, fakePointer(0x1105));
    EXPECT_EQ(create_info->flags, 0u);
    *semaphore = state.semaphore;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroySemaphore(const VkDevice device, const VkSemaphore semaphore, const VkAllocationCallbacks * allocator)
{
    ++state.destroy_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(semaphore, state.semaphore);
    EXPECT_EQ(allocator, nullptr);
}

VkResult VKAPI_CALL
createShaderModule(const VkDevice                   device,
                   const VkShaderModuleCreateInfo * create_info,
                   const VkAllocationCallbacks *    allocator,
                   VkShaderModule *                 shader_module)
{
    ++state.create_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_info->pNext, fakePointer(0x1106));
    EXPECT_EQ(create_info->codeSize, sizeof(std::uint32_t) * 3u);
    EXPECT_EQ(create_info->pCode[0], 0x07230203u);
    EXPECT_EQ(create_info->pCode[2], 0x55667788u);
    *shader_module = state.shader_module;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroyShaderModule(const VkDevice device, const VkShaderModule shader_module, const VkAllocationCallbacks * allocator)
{
    ++state.destroy_calls;
    EXPECT_EQ(device, state.device);
    EXPECT_EQ(shader_module, state.shader_module);
    EXPECT_EQ(allocator, nullptr);
}

}  // namespace

TEST(VulkanFunctionsResourceTest, BufferForwardsOperationsAndDestroys)
{
    state = {};
    vkwp::VulkanFunctions functions;
    functions.vkCreateBuffer                = &createBuffer;
    functions.vkDestroyBuffer               = &destroyBuffer;
    functions.vkGetBufferMemoryRequirements = &getBufferMemoryRequirements;
    functions.vkBindBufferMemory            = &bindBufferMemory;
    functions.vkGetBufferDeviceAddress      = &getBufferDeviceAddress;

    const auto device = vkwp::Device::importExternal("device", state.device, &functions);
    const auto memory = vkwp::DeviceMemory::importExternal("memory", device, state.memory, &functions);
    {
        const vkwp::Buffer::OptionalCreateArgs optional_args{
            .p_next      = fakePointer(0x1101),
            .flags       = VK_BUFFER_CREATE_SPARSE_BINDING_BIT,
            .debug_utils = nullptr,
            .functions   = &functions,
        };
        auto buffer
            = vkwp::Buffer::create("buffer",
                                   device,
                                   4096,
                                   VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                                   optional_args);
        EXPECT_EQ(buffer.getMemoryRequirements().alignment, 256u);
        buffer.bindMemory(memory, 512);
        EXPECT_EQ(buffer.getBufferDeviceAddress(), 0x12345678u);
        EXPECT_EQ(state.operation_calls, 3);
    }
    EXPECT_EQ(state.create_calls, 1);
    EXPECT_EQ(state.destroy_calls, 1);
}

TEST(VulkanFunctionsResourceTest, ImportedBufferForwardsOperationsWithoutDestroying)
{
    state = {};
    vkwp::VulkanFunctions functions;
    functions.vkDestroyBuffer               = &destroyBuffer;
    functions.vkGetBufferMemoryRequirements = &getBufferMemoryRequirements;
    functions.vkBindBufferMemory            = &bindBufferMemory;
    functions.vkGetBufferDeviceAddress      = &getBufferDeviceAddress;

    const auto device = vkwp::Device::importExternal("device", state.device, &functions);
    const auto memory = vkwp::DeviceMemory::importExternal("memory", device, state.memory, &functions);
    {
        const auto buffer = vkwp::Buffer::importExternal("buffer", device, state.buffer, &functions);
        EXPECT_TRUE(buffer.isExternal());
        EXPECT_EQ(buffer.getMemoryRequirements().alignment, 256u);
        buffer.bindMemory(memory, 512);
        EXPECT_EQ(buffer.getBufferDeviceAddress(), 0x12345678u);
        EXPECT_EQ(state.operation_calls, 3);
    }
    EXPECT_EQ(state.create_calls, 0);
    EXPECT_EQ(state.destroy_calls, 0);
}

TEST(VulkanFunctionsResourceTest, ImageForwardsOperationsAndDestroys)
{
    state = {};
    vkwp::VulkanFunctions functions;
    functions.vkCreateImage                 = &createImage;
    functions.vkDestroyImage                = &destroyImage;
    functions.vkGetImageMemoryRequirements  = &getImageMemoryRequirements;
    functions.vkGetImageMemoryRequirements2 = &getImageMemoryRequirements2;
    functions.vkBindImageMemory             = &bindImageMemory;

    const auto device = vkwp::Device::importExternal("device", state.device, &functions);
    const auto memory = vkwp::DeviceMemory::importExternal("memory", device, state.memory, &functions);
    {
        const vkwp::Image::OptionalCreateArgs optional_args{
            .p_next       = fakePointer(0x1102),
            .flags        = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
            .mip_levels   = 4,
            .array_layers = 6,
            .samples      = VK_SAMPLE_COUNT_4_BIT,
            .tiling       = VK_IMAGE_TILING_LINEAR,
            .debug_utils  = nullptr,
            .functions    = &functions,
        };
        auto image = vkwp::Image::create2D("image",
                                           device,
                                           VK_FORMAT_R8G8B8A8_UNORM,
                                           640,
                                           480,
                                           VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                           optional_args);
        EXPECT_EQ(image.getMemoryRequirements().size, 16384u);
        EXPECT_EQ(image.getMemoryRequirements2(fakePointer(0x1103)).memoryRequirements.alignment, 1024u);
        image.bindMemory(memory, 2048);
        EXPECT_EQ(state.operation_calls, 3);
    }
    EXPECT_EQ(state.create_calls, 1);
    EXPECT_EQ(state.destroy_calls, 1);
}

TEST(VulkanFunctionsResourceTest, ImageViewForwardsCreateInfoAndDestroys)
{
    state = {};
    vkwp::VulkanFunctions functions;
    functions.vkCreateImageView          = &createImageView;
    functions.vkDestroyImageView         = &destroyImageView;
    const auto                    device = vkwp::Device::importExternal("device", state.device, &functions);
    const auto                    image  = vkwp::Image::importExternal("image", device, state.image, &functions);
    const VkImageSubresourceRange range{
        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel   = 1,
        .levelCount     = 4,
        .baseArrayLayer = 2,
        .layerCount     = 6,
    };
    vkwp::ImageView::OptionalCreateArgs optional_args;
    optional_args.components.r = VK_COMPONENT_SWIZZLE_B;
    optional_args.functions    = &functions;
    {
        const auto view = vkwp::ImageView::create("view",
                                                  device,
                                                  image,
                                                  VK_IMAGE_VIEW_TYPE_2D_ARRAY,
                                                  VK_FORMAT_R8G8B8A8_UNORM,
                                                  range,
                                                  optional_args);
        EXPECT_EQ(static_cast<VkImageView>(view), state.image_view);
    }
    EXPECT_EQ(state.create_calls, 1);
    EXPECT_EQ(state.destroy_calls, 1);
}

TEST(VulkanFunctionsResourceTest, RenderPassAndFramebufferForwardCreateInfoAndDestroy)
{
    state = {};
    vkwp::VulkanFunctions functions;
    functions.vkCreateRenderPass         = &createRenderPass;
    functions.vkDestroyRenderPass        = &destroyRenderPass;
    functions.vkCreateFramebuffer        = &createFramebuffer;
    functions.vkDestroyFramebuffer       = &destroyFramebuffer;
    const auto                    device = vkwp::Device::importExternal("device", state.device, &functions);
    const VkAttachmentDescription attachment{
        .flags          = 0,
        .format         = VK_FORMAT_R8G8B8A8_UNORM,
        .samples        = static_cast<VkSampleCountFlagBits>(0),
        .loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD,
        .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_LOAD,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE,
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = VK_IMAGE_LAYOUT_UNDEFINED,
    };
    const VkSubpassDescription subpass{
        .flags                   = 0,
        .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount    = 0,
        .pInputAttachments       = nullptr,
        .colorAttachmentCount    = 0,
        .pColorAttachments       = nullptr,
        .pResolveAttachments     = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments    = nullptr,
    };
    const VkSubpassDependency dependency{
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0,
        .srcStageMask    = 0,
        .dstStageMask    = 0,
        .srcAccessMask   = 0,
        .dstAccessMask   = 0,
        .dependencyFlags = 0,
    };
    {
        const vkwp::RenderPass::OptionalCreateArgs render_pass_args{
            .functions = &functions,
        };
        auto       render_pass     = vkwp::RenderPass::create("render-pass",
                                                    device,
                                                              { attachment },
                                                              { subpass },
                                                              { dependency },
                                                    render_pass_args);
        const auto attachment_view = vkwp::ImageView::importExternal("view", device, state.image_view);
        const vkwp::Framebuffer::OptionalCreateArgs framebuffer_args{
            .functions = &functions,
        };
        {
            const auto framebuffer = vkwp::Framebuffer::create("framebuffer",
                                                               device,
                                                               render_pass,
                                                               { std::cref(attachment_view) },
                                                               640,
                                                               480,
                                                               2,
                                                               framebuffer_args);
            EXPECT_EQ(static_cast<VkFramebuffer>(framebuffer), state.framebuffer);
        }
        EXPECT_EQ(state.destroy_calls, 1);
    }
    EXPECT_EQ(state.create_calls, 2);
    EXPECT_EQ(state.destroy_calls, 2);
}

TEST(VulkanFunctionsResourceTest, FenceForwardsOperationsAndDestroys)
{
    state = {};
    vkwp::VulkanFunctions functions;
    functions.vkCreateFence   = &createFence;
    functions.vkDestroyFence  = &destroyFence;
    functions.vkWaitForFences = &waitForFences;
    functions.vkResetFences   = &resetFences;
    const auto device         = vkwp::Device::importExternal("device", state.device, &functions);
    {
        const vkwp::Fence::OptionalCreateArgs optional_args{
            .p_next      = fakePointer(0x1104),
            .flags       = VK_FENCE_CREATE_SIGNALED_BIT,
            .debug_utils = nullptr,
            .functions   = &functions,
        };
        auto fence = vkwp::Fence::create("fence", device, optional_args);
        EXPECT_TRUE(fence.wait(12345));
        fence.resetSignal();
    }
    EXPECT_EQ(state.create_calls, 1);
    EXPECT_EQ(state.operation_calls, 2);
    EXPECT_EQ(state.destroy_calls, 1);
}

TEST(VulkanFunctionsResourceTest, SamplerSemaphoreAndShaderModuleDestroyWithInjectedFunctions)
{
    state = {};
    vkwp::VulkanFunctions functions;
    functions.vkCreateSampler       = &createSampler;
    functions.vkDestroySampler      = &destroySampler;
    functions.vkCreateSemaphore     = &createSemaphore;
    functions.vkDestroySemaphore    = &destroySemaphore;
    functions.vkCreateShaderModule  = &createShaderModule;
    functions.vkDestroyShaderModule = &destroyShaderModule;
    const auto device               = vkwp::Device::importExternal("device", state.device, &functions);
    {
        const vkwp::Sampler::OptionalCreateArgs sampler_args{
            .anisotropy_enable = true,
            .max_anisotropy    = 8.0f,
            .functions         = &functions,
        };
        const auto sampler = vkwp::Sampler::create("sampler",
                                                   device,
                                                   VK_FILTER_LINEAR,
                                                   VK_FILTER_NEAREST,
                                                   VK_SAMPLER_MIPMAP_MODE_LINEAR,
                                                   VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                                   VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                                                   VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
                                                   sampler_args);

        const vkwp::Semaphore::OptionalCreateArgs semaphore_args{
            .p_next    = fakePointer(0x1105),
            .functions = &functions,
        };
        const auto semaphore = vkwp::Semaphore::create("semaphore", device, semaphore_args);

        const std::vector<std::uint32_t>             code{ 0x07230203u, 0x11223344u, 0x55667788u };
        const vkwp::ShaderModule::OptionalCreateArgs shader_args{
            .p_next    = fakePointer(0x1106),
            .functions = &functions,
        };
        const auto shader = vkwp::ShaderModule::create("shader", device, code, shader_args);
        EXPECT_TRUE(sampler);
        EXPECT_TRUE(semaphore);
        EXPECT_TRUE(shader);
    }
    EXPECT_EQ(state.create_calls, 3);
    EXPECT_EQ(state.destroy_calls, 3);
}
