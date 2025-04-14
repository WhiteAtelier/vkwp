
option(VKWP_SUPPORT_GLFW_SURFACE "GLFW surface support." )
if (WIN32)
    option(VKWP_SUPPORT_WIN32_SURFACE "WIN32 surface support." OFF)
else()
    set(VKWP_SUPPORT_WIN32_SURFACE OFF)
endif()

vkwp_add_library(vkwp
    LIBRARIES
        Vulkan::Vulkan
   
    SOURCES
        buffer.cpp
        command_buffer.cpp
        command_pool.cpp
        debug_report_callback_resource.cpp
        descriptor_pool.cpp
        descriptor_set_layout.cpp
        descriptor_set.cpp
        device_memory_node.cpp
        device_memory_pool.cpp
        device_memory.cpp
        device.cpp
        fence.cpp
        framebuffer.cpp
        image_view.cpp
        image.cpp
        instance.cpp
        physical_device.cpp
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
        impl/device_memory_pool_unit.cpp
        impl/device_memory_pool_unit.hpp

    HEADERS
        vkwp/buffer.hpp
        vkwp/command_buffer.hpp
        vkwp/command_pool.hpp
        vkwp/debug_report_callback_resource.hpp
        vkwp/descriptor_pool.hpp
        vkwp/descriptor_set_layout.hpp
        vkwp/descriptor_set.hpp
        vkwp/device_memory_node.hpp
        vkwp/device_memory_pool.hpp
        vkwp/device_memory.hpp
        vkwp/device.hpp
        vkwp/fence.hpp
        vkwp/framebuffer.hpp
        vkwp/fwd.hpp
        vkwp/image_view.hpp
        vkwp/image.hpp
        vkwp/instance.hpp
        vkwp/physical_device.hpp
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
        vkwp/vulkan_error.hpp
        vkwp/vulkan.hpp

    MSVC_HEADERS_FILTER_ROOT
        vkwp

    TESTS
        main.cpp
)

if (${VKWP_SUPPORT_GLFW_SURFACE})
    find_package(glfw3 REQUIRED)
    target_link_libraries(vkwp
        PRIVATE
            glfw
    )
endif()

if (${VKWP_SPDLOG_LOGGING})
    target_link_libraries(vkwp
        PRIVATE
            spdlog::spdlog
    )
endif()

target_compile_definitions(vkwp
    PUBLIC
        VKWP_SUPPORT_GLFW_SURFACE=$<BOOL:${VKWP_SUPPORT_GLFW_SURFACE}>
        VKWP_SUPPORT_WIN32_SURFACE=$<BOOL:${VKWP_SUPPORT_WIN32_SURFACE}>
)

