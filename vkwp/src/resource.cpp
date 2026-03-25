#include "vkwp/resource.hpp"

#include "impl/deleter.hpp"
#include "vkwp/vulkan.hpp"

template <::vkwp::vulkan_raw_resource T, typename... ParentTs>
vkwp::Resource<T, ParentTs...>::Resource()
    : label_{}
    , instance_{ nullptr }
    , external_{ false }
    , parents_{}
{}

template <::vkwp::vulkan_raw_resource T, typename... ParentTs>
vkwp::Resource<T, ParentTs...>::Resource(const std::string_view label)
    : label_{ label }
    , instance_{ nullptr }
    , external_{ false }
    , parents_{}
{}

template <::vkwp::vulkan_raw_resource T, typename... ParentTs>
vkwp::Resource<T, ParentTs...>::Resource(const std::string_view label, const T & external)
    : label_{ label }
    , instance_{ external }
    , external_{ true }
    , parents_{}
{}

template <::vkwp::vulkan_raw_resource T, typename... ParentTs>
vkwp::Resource<T, ParentTs...>::Resource(Resource && m) noexcept
    : label_{ std::move(m.label_) }
    , instance_{ m.instance_ }
    , external_{ m.external_ }
    , parents_{ std::move(m.parents_) }
{
    if (this != &m)
    {
        m.external_ = false;
        m.instance_ = nullptr;
        m.parents_  = std::tuple<ParentTs...>{};
    }
}

template <::vkwp::vulkan_raw_resource T, typename... ParentTs>
vkwp::Resource<T, ParentTs...>::~Resource() noexcept
{
    if (this->instance_ != nullptr && !this->external_)
    {
        impl::_delete<T, ParentTs...>(this->instance_, this->parents_);
    }
}

template <::vkwp::vulkan_raw_resource T, typename... ParentTs>
vkwp::Resource<T, ParentTs...> &
vkwp::Resource<T, ParentTs...>::operator=(Resource && rhs) & noexcept
{
    if (this != &rhs)
    {
        // Reset
        if (this->instance_ != nullptr && !this->external_)
        {
            impl::_delete<T, ParentTs...>(this->instance_, this->parents_);
        }

        this->label_    = std::move(rhs.label_);
        this->instance_ = rhs.instance_;
        this->external_ = rhs.external_;
        this->parents_  = std::move(rhs.parents_);
        rhs.instance_   = nullptr;
        rhs.external_   = false;
        rhs.parents_    = std::tuple<ParentTs...>{};
    }
    return *this;
}

template <::vkwp::vulkan_raw_resource T, typename... ParentTs>
vkwp::Resource<T, ParentTs...>::operator bool() const & noexcept
{
    return this->instance_ != nullptr;
}

template <::vkwp::vulkan_raw_resource T, typename... ParentTs>
bool
vkwp::Resource<T, ParentTs...>::operator!() const & noexcept
{
    return !static_cast<bool>(*this);
}

template <::vkwp::vulkan_raw_resource T, typename... ParentTs>
const std::string &
vkwp::Resource<T, ParentTs...>::getLabel() const & noexcept
{
    return this->label_;
}

template <::vkwp::vulkan_raw_resource T, typename... ParentTs>
bool
vkwp::Resource<T, ParentTs...>::isExternal() const & noexcept
{
    return this->external_;
}

template <::vkwp::vulkan_raw_resource T, typename... ParentTs>
vkwp::Resource<T, ParentTs...>::operator T() const & noexcept
{
    return this->instance_;
}

template <::vkwp::vulkan_raw_resource T, typename... ParentTs>
const T *
vkwp::Resource<T, ParentTs...>::ptr() const & noexcept
{
    return &this->instance_;
}

template <::vkwp::vulkan_raw_resource T, typename... ParentTs>
void
vkwp::Resource<T, ParentTs...>::reset() & noexcept
{
    if (this->instance_ != nullptr)
    {
        if (!this->external_)
        {
            impl::_delete<T, ParentTs...>(this->instance_, this->parents_);
        }
        this->instance_ = nullptr;
        this->parents_  = std::tuple<ParentTs...>{};
        this->external_ = false;
        this->label_.clear();
    }
}

template <::vkwp::vulkan_raw_resource T, typename... ParentTs>
T *
vkwp::Resource<T, ParentTs...>::_reset(ParentTs... parents) & noexcept
{
    this->reset();
    this->parents_ = std::make_tuple<ParentTs...>(std::forward<ParentTs>(parents)...);
    return &this->instance_;
}

#include "vkwp/acceleration_structure_khr.hpp"

template class vkwp::Resource<VkAccelerationStructureKHR, VkDevice, vkwp::AccelerationStructureKHR_ExtFunctions>;
template class vkwp::Resource<VkBuffer, VkDevice>;
template class vkwp::Resource<VkCommandBuffer, VkDevice, VkCommandPool>;
template class vkwp::Resource<VkCommandPool, VkDevice>;
template class vkwp::Resource<VkDescriptorPool, VkDevice>;
template class vkwp::Resource<VkDescriptorSet, VkDevice, VkDescriptorPool>;
template class vkwp::Resource<VkDescriptorSetLayout, VkDevice>;
template class vkwp::Resource<VkDevice>;
template class vkwp::Resource<VkDeviceMemory, VkDevice>;
template class vkwp::Resource<VkFence, VkDevice>;
template class vkwp::Resource<VkFramebuffer, VkDevice>;
template class vkwp::Resource<VkImage, VkDevice>;
template class vkwp::Resource<VkImageView, VkDevice>;
template class vkwp::Resource<VkInstance>;
template class vkwp::Resource<VkPhysicalDevice>;
template class vkwp::Resource<VkPipeline, VkDevice>;
template class vkwp::Resource<VkPipelineCache, VkDevice>;
template class vkwp::Resource<VkPipelineLayout, VkDevice>;
template class vkwp::Resource<VkQueue>;
template class vkwp::Resource<VkRenderPass, VkDevice>;
template class vkwp::Resource<VkSampler, VkDevice>;
template class vkwp::Resource<VkSemaphore, VkDevice>;
template class vkwp::Resource<VkShaderModule, VkDevice>;
template class vkwp::Resource<VkSurfaceKHR, VkInstance>;
template class vkwp::Resource<VkSwapchainKHR, VkDevice>;
