#include "vkwp/resource.hpp"

#include "impl/deleter.hpp"

#include "vkwp/vulkan.hpp"

template <::vkwp::vulkan_raw_resource T, typename DFn, typename... ParentTs>
vkwp::Resource<T, DFn, ParentTs...>::Resource()
    : label_{}
    , instance_{ nullptr }
    , destroy_fn_{ nullptr }
    , external_{ false }
    , parents_{}
{}

template <::vkwp::vulkan_raw_resource T, typename DFn, typename... ParentTs>
vkwp::Resource<T, DFn, ParentTs...>::Resource(std::string && label)
    : label_{ std::move(label) }
    , instance_{ nullptr }
    , destroy_fn_{ nullptr }
    , external_{ false }
    , parents_{}
{}

template <::vkwp::vulkan_raw_resource T, typename DFn, typename... ParentTs>
vkwp::Resource<T, DFn, ParentTs...>::Resource(std::string && label, const T & external)
    : label_{ std::move(label) }
    , instance_{ external }
    , destroy_fn_{ nullptr }
    , external_{ true }
    , parents_{}
{}

template <::vkwp::vulkan_raw_resource T, typename DFn, typename... ParentTs>
vkwp::Resource<T, DFn, ParentTs...>::Resource(Resource && m) noexcept
    : label_{ std::move(m.label_) }
    , instance_{ m.instance_ }
    , destroy_fn_{ m.destroy_fn_ }
    , external_{ m.external_ }
    , parents_{ std::move(m.parents_) }
{
    if (this != &m)
    {
        m.external_   = false;
        m.instance_   = nullptr;
        m.destroy_fn_ = nullptr;
        m.parents_    = std::tuple<ParentTs...>{};
    }
}

template <::vkwp::vulkan_raw_resource T, typename DFn, typename... ParentTs>
vkwp::Resource<T, DFn, ParentTs...>::~Resource() noexcept
{
    if (this->instance_ != nullptr && !this->external_)
    {
        impl::_delete<T, DFn, ParentTs...>(this->instance_, this->destroy_fn_, this->parents_);
    }
}

template <::vkwp::vulkan_raw_resource T, typename DFn, typename... ParentTs>
vkwp::Resource<T, DFn, ParentTs...> &
vkwp::Resource<T, DFn, ParentTs...>::operator=(Resource && rhs) & noexcept
{
    if (this != &rhs)
    {
        // Reset
        if (this->instance_ != nullptr && !this->external_)
        {
            impl::_delete<T, DFn, ParentTs...>(this->instance_, this->destroy_fn_, this->parents_);
        }

        this->label_      = std::move(rhs.label_);
        this->instance_   = rhs.instance_;
        this->destroy_fn_ = rhs.destroy_fn_;
        this->external_   = rhs.external_;
        this->parents_    = std::move(rhs.parents_);
        rhs.destroy_fn_   = nullptr;
        rhs.instance_     = nullptr;
        rhs.external_     = false;
        rhs.parents_      = std::tuple<ParentTs...>{};
    }
    return *this;
}

template <::vkwp::vulkan_raw_resource T, typename DFn, typename... ParentTs>
vkwp::Resource<T, DFn, ParentTs...>::operator bool() const & noexcept
{
    return this->instance_ != nullptr;
}

template <::vkwp::vulkan_raw_resource T, typename DFn, typename... ParentTs>
bool
vkwp::Resource<T, DFn, ParentTs...>::operator!() const & noexcept
{
    return !static_cast<bool>(*this);
}

template <::vkwp::vulkan_raw_resource T, typename DFn, typename... ParentTs>
const std::string &
vkwp::Resource<T, DFn, ParentTs...>::getLabel() const & noexcept
{
    return this->label_;
}

template <::vkwp::vulkan_raw_resource T, typename DFn, typename... ParentTs>
bool
vkwp::Resource<T, DFn, ParentTs...>::isExternal() const & noexcept
{
    return this->external_;
}

template <::vkwp::vulkan_raw_resource T, typename DFn, typename... ParentTs>
vkwp::Resource<T, DFn, ParentTs...>::operator T() const & noexcept
{
    return this->instance_;
}

template <::vkwp::vulkan_raw_resource T, typename DFn, typename... ParentTs>
const T *
vkwp::Resource<T, DFn, ParentTs...>::ptr() const & noexcept
{
    return &this->instance_;
}

template <::vkwp::vulkan_raw_resource T, typename DFn, typename... ParentTs>
void
vkwp::Resource<T, DFn, ParentTs...>::reset() & noexcept
{
    if (this->instance_ != nullptr)
    {
        if (!this->external_)
        {
            impl::_delete<T, DFn, ParentTs...>(this->instance_, this->destroy_fn_, this->parents_);
        }
        this->destroy_fn_ = nullptr;
        this->instance_   = nullptr;
        this->parents_    = std::tuple<ParentTs...>{};
        this->external_   = false;
        this->label_.clear();
    }
}

template <::vkwp::vulkan_raw_resource T, typename DFn, typename... ParentTs>
T *
vkwp::Resource<T, DFn, ParentTs...>::_reset(DFn deleter_fn, ParentTs... parents) & noexcept
{
    this->reset();
    this->destroy_fn_ = deleter_fn;
    this->parents_    = std::make_tuple<ParentTs...>(std::forward<ParentTs>(parents)...);
    return &this->instance_;
}

#include "vkwp/acceleration_structure_khr.hpp"

template class vkwp::Resource<VkAccelerationStructureKHR, PFN_vkDestroyAccelerationStructureKHR, VkDevice>;
template class vkwp::Resource<VkBuffer, PFN_vkDestroyBuffer, VkDevice>;
template class vkwp::Resource<VkCommandBuffer, PFN_vkFreeCommandBuffers, VkDevice, VkCommandPool>;
template class vkwp::Resource<VkCommandPool, PFN_vkDestroyCommandPool, VkDevice>;
template class vkwp::Resource<VkDescriptorPool, PFN_vkDestroyDescriptorPool, VkDevice>;
template class vkwp::Resource<VkDescriptorSet, PFN_vkFreeDescriptorSets, VkDevice, VkDescriptorPool>;
template class vkwp::Resource<VkDescriptorSetLayout, PFN_vkDestroyDescriptorSetLayout, VkDevice>;
template class vkwp::Resource<VkDevice, PFN_vkDestroyDevice>;
template class vkwp::Resource<VkDeviceMemory, PFN_vkFreeMemory, VkDevice>;
template class vkwp::Resource<VkFence, PFN_vkDestroyFence, VkDevice>;
template class vkwp::Resource<VkFramebuffer, PFN_vkDestroyFramebuffer, VkDevice>;
template class vkwp::Resource<VkImage, PFN_vkDestroyImage, VkDevice>;
template class vkwp::Resource<VkImageView, PFN_vkDestroyImageView, VkDevice>;
template class vkwp::Resource<VkInstance, PFN_vkDestroyInstance>;
template class vkwp::Resource<VkPhysicalDevice, std::nullptr_t>;
template class vkwp::Resource<VkPipeline, PFN_vkDestroyPipeline, VkDevice>;
template class vkwp::Resource<VkPipelineCache, PFN_vkDestroyPipelineCache, VkDevice>;
template class vkwp::Resource<VkPipelineLayout, PFN_vkDestroyPipelineLayout, VkDevice>;
template class vkwp::Resource<VkQueue, std::nullptr_t>;
template class vkwp::Resource<VkRenderPass, PFN_vkDestroyRenderPass, VkDevice>;
template class vkwp::Resource<VkSampler, PFN_vkDestroySampler, VkDevice>;
template class vkwp::Resource<VkSemaphore, PFN_vkDestroySemaphore, VkDevice>;
template class vkwp::Resource<VkShaderModule, PFN_vkDestroyShaderModule, VkDevice>;
template class vkwp::Resource<VkSurfaceKHR, PFN_vkDestroySurfaceKHR, VkInstance>;
template class vkwp::Resource<VkSwapchainKHR, PFN_vkDestroySwapchainKHR, VkDevice>;
