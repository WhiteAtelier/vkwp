# Copyright (c) 2026 Shirao Shotaro, WhiteAtelier
# SPDX-License-Identifier: MIT
#
# See the LICENSE file in the project root for the full license text.

option(VKWP_SUPPORT_GLFW_SURFACE "GLFW surface support." )
if (WIN32)
    option(VKWP_SUPPORT_WIN32_SURFACE "WIN32 surface support." OFF)
else()
    set(VKWP_SUPPORT_WIN32_SURFACE OFF)
endif()

vkwp_add_library(vkwp
    LIBRARIES
        spdlog::spdlog
        Vulkan::Vulkan

    SOURCES
        acceleration_structure_khr.cpp
        buffer.cpp
        command_buffer.cpp
        command_pool.cpp
        debug_report_callback_resource.cpp
        debug_utils_resource.cpp
        descriptor_pool.cpp
        descriptor_set_layout.cpp
        descriptor_set.cpp
        device_memory.cpp
        device.cpp
        fence.cpp
        framebuffer.cpp
        image_view.cpp
        image.cpp
        instance.cpp
        mutex.cpp
        physical_device.cpp
        pipeline_cache.cpp
        pipeline_layout.cpp
        pipeline.cpp
        queue.cpp
        render_pass.cpp
        resource.cpp
        sampler.cpp
        semaphore.cpp
        shader_module.cpp
        surface_khr.cpp
        swapchain_khr.cpp
        vulkan_error.cpp

        impl/assert.cpp
        impl/assert.hpp
        impl/deleter.hpp

    HEADERS
        vkwp/acceleration_structure_khr.hpp
        vkwp/buffer.hpp
        vkwp/command_buffer.hpp
        vkwp/command_pool.hpp
        vkwp/debug_report_callback_resource.hpp
        vkwp/debug_utils_resource.hpp
        vkwp/descriptor_pool.hpp
        vkwp/descriptor_set_layout.hpp
        vkwp/descriptor_set.hpp
        vkwp/device_memory.hpp
        vkwp/device.hpp
        vkwp/fence.hpp
        vkwp/framebuffer.hpp
        vkwp/fwd.hpp
        vkwp/image_view.hpp
        vkwp/image.hpp
        vkwp/instance.hpp
        vkwp/mutex.hpp
        vkwp/physical_device.hpp
        vkwp/pipeline_cache.hpp
        vkwp/pipeline_layout.hpp
        vkwp/pipeline.hpp
        vkwp/queue.hpp
        vkwp/render_pass.hpp
        vkwp/resource.hpp
        vkwp/sampler.hpp
        vkwp/semaphore.hpp
        vkwp/shader_module.hpp
        vkwp/surface_khr.hpp
        vkwp/swapchain_khr.hpp
        vkwp/types.hpp
        vkwp/utils.hpp
        vkwp/vk_structures.hpp
        vkwp/vulkan_error.hpp
        vkwp/vulkan_functions.hpp
        vkwp/vulkan.hpp

    MSVC_HEADERS_FILTER_ROOT
        vkwp

    TESTS
        main.cpp
        core_test.cpp
        resource_lifecycle_test.cpp
        command_and_memory_test.cpp
        pipeline_and_debug_test.cpp
        vulkan_error_test.cpp
        test_utils.hpp
)

if (${VKWP_SUPPORT_GLFW_SURFACE})
    find_package(glfw3 REQUIRED)
    target_link_libraries(vkwp
        PRIVATE
            glfw
    )
endif()

target_compile_definitions(vkwp
    PUBLIC
        VKWP_SUPPORT_GLFW_SURFACE=$<BOOL:${VKWP_SUPPORT_GLFW_SURFACE}>
        VKWP_SUPPORT_WIN32_SURFACE=$<BOOL:${VKWP_SUPPORT_WIN32_SURFACE}>
)

target_compile_definitions(vkwp
    PRIVATE
        VKWP_USE_GLFW_SURFACE
        VKWP_USE_WIN32_SURFACE
)
