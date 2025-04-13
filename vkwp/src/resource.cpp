#include "impl/assert.hpp"
#include "vkwp/vulkan.hpp"
#include "vkwp/vulkan_error.hpp"

#include <vulkan/vulkan.h>


namespace {

#define SFL_DEFINE_DELETER(VkType, VkParentType, VkPoolType) \
    void _deleter(const VkType & i, const VkParentType * pa, const VkPoolType * po) noexcept

// clang-format off
SFL_DEFINE_DELETER(VkInstance,            void,       void)             { vkDestroyInstance(i, nullptr); }
SFL_DEFINE_DELETER(VkPhysicalDevice,      void,       void)             {}
SFL_DEFINE_DELETER(VkDevice,              void,       void)             { vkDestroyDevice(i, nullptr); }
SFL_DEFINE_DELETER(VkQueue,               void,       void)             {}
SFL_DEFINE_DELETER(VkImage,               VkDevice,   void)             { vkDestroyImage(*pa, i, nullptr); }
SFL_DEFINE_DELETER(VkImageView,           VkDevice,   void)             { vkDestroyImageView(*pa, i, nullptr); }
SFL_DEFINE_DELETER(VkDeviceMemory,        VkDevice,   void)             { vkFreeMemory(*pa, i, nullptr); }
SFL_DEFINE_DELETER(VkSurfaceKHR,          VkInstance, void)             { vkDestroySurfaceKHR(*pa, i, nullptr); }
SFL_DEFINE_DELETER(VkSwapchainKHR,        VkDevice,   void)             { vkDestroySwapchainKHR(*pa, i, nullptr); }
SFL_DEFINE_DELETER(VkDescriptorPool,      VkDevice,   void)             { vkDestroyDescriptorPool(*pa, i, nullptr); }
SFL_DEFINE_DELETER(VkDescriptorSetLayout, VkDevice,   void)             { vkDestroyDescriptorSetLayout(*pa, i, nullptr); }
SFL_DEFINE_DELETER(VkDescriptorSet,       VkDevice,   VkDescriptorPool) { vkFreeDescriptorSets(*pa, *po, 1, &i); }
SFL_DEFINE_DELETER(VkPipelineLayout,      VkDevice,   void)             { vkDestroyPipelineLayout(*pa, i, nullptr); }
SFL_DEFINE_DELETER(VkPipeline,            VkDevice,   void)             { vkDestroyPipeline(*pa, i, nullptr); }
SFL_DEFINE_DELETER(VkRenderPass,          VkDevice,   void)             { vkDestroyRenderPass(*pa, i, nullptr); }
SFL_DEFINE_DELETER(VkFramebuffer,         VkDevice,   void)             { vkDestroyFramebuffer(*pa, i, nullptr); }
SFL_DEFINE_DELETER(VkSampler,             VkDevice,   void)             { vkDestroySampler(*pa, i, nullptr); }
SFL_DEFINE_DELETER(VkShaderModule,        VkDevice,   void)             { vkDestroyShaderModule(*pa, i, nullptr); }
SFL_DEFINE_DELETER(VkCommandPool,         VkDevice,   void)             { vkDestroyCommandPool(*pa, i, nullptr); }
SFL_DEFINE_DELETER(VkCommandBuffer,       VkDevice,   VkCommandPool)    { vkFreeCommandBuffers(*pa, *po, 1, &i); }
SFL_DEFINE_DELETER(VkBuffer,              VkDevice,   void)             { vkDestroyBuffer(*pa, i, nullptr); }
SFL_DEFINE_DELETER(VkFence,               VkDevice,   void)             { vkDestroyFence(*pa, i, nullptr); }
SFL_DEFINE_DELETER(VkSemaphore,           VkDevice,   void)             { vkDestroySemaphore(*pa, i, nullptr); }
// clang-format on

#undef DEFINE_DELETER

}  // namespace

template <::vkwp::vulkan_raw_resource T>
vkwp::ResourceBase<T>::ResourceBase()
    : label_{}
    , instance_{ VK_NULL_HANDLE }
{}

template <::vkwp::vulkan_raw_resource T>
vkwp::ResourceBase<T>::ResourceBase(const std::string & label)
    : label_{ label }
    , instance_{ VK_NULL_HANDLE }
{}

template <::vkwp::vulkan_raw_resource T>
vkwp::ResourceBase<T>::ResourceBase(ResourceBase<T> && m) noexcept
    : label_{ std::move(m.label_) }
    , instance_{ m.instance_ }
{
    m.instance_ = VK_NULL_HANDLE;
}

template <::vkwp::vulkan_raw_resource T>
vkwp::ResourceBase<T>::~ResourceBase() noexcept = default;

template <::vkwp::vulkan_raw_resource T>
vkwp::ResourceBase<T> &
vkwp::ResourceBase<T>::operator=(ResourceBase<T> && rhs) & noexcept
{
    static_assert(std::is_nothrow_move_assignable_v<std::string>);
    static_assert(std::is_nothrow_copy_assignable_v<T>);
    if (this != &rhs)
    {
        this->label_    = std::move(rhs.label_);
        this->instance_ = rhs.instance_;
        rhs.instance_   = nullptr;
    }
    return *this;
}

template <::vkwp::vulkan_raw_resource T>
bool
vkwp::ResourceBase<T>::operator!() const & noexcept
{
    return this->instance_ == nullptr;
}

template <::vkwp::vulkan_raw_resource T>
vkwp::ResourceBase<T>::operator bool() const & noexcept
{
    return this->instance_ != nullptr;
}

template <::vkwp::vulkan_raw_resource T>
vkwp::ResourceBase<T>::operator T() const & noexcept
{
    static_assert(std::is_nothrow_copy_assignable_v<T>);
    return this->instance_;
}

template <::vkwp::vulkan_raw_resource T>
const T *
vkwp::ResourceBase<T>::ptr() const & noexcept
{
    return &this->instance_;
}

template <::vkwp::vulkan_raw_resource T>
const std::string &
vkwp::ResourceBase<T>::getLabel() const & noexcept
{
    return this->label_;
}

template <::vkwp::vulkan_raw_resource T>
void
vkwp::ResourceBase<T>::setLabel(const std::string & label) &
{
    this->label_ = label;
}

template <::vkwp::vulkan_raw_resource T>
const T &
vkwp::ResourceBase<T>::_ref() const noexcept
{
    return this->instance_;
}

template <::vkwp::vulkan_raw_resource T>
T &
vkwp::ResourceBase<T>::_ref() noexcept
{
    return this->instance_;
}

// ============================================================================================= //
// Resource<T, ParentT, PoolT>
// ============================================================================================= //
template <::vkwp::vulkan_raw_resource         T,
          ::vkwp::vulkan_raw_resource_or_void ParentT,
          ::vkwp::vulkan_raw_resource_or_void PoolT>
vkwp::Resource<T, ParentT, PoolT>::Resource()
    : ResourceBase<T>{}
    , parent_{ nullptr }
    , pool_{ nullptr }
{}

template <::vkwp::vulkan_raw_resource         T,
          ::vkwp::vulkan_raw_resource_or_void ParentT,
          ::vkwp::vulkan_raw_resource_or_void PoolT>
vkwp::Resource<T, ParentT, PoolT>::Resource(const std::string & label)
    : ResourceBase<T>{ label }
    , parent_{ nullptr }
    , pool_{ nullptr }
{}

template <::vkwp::vulkan_raw_resource         T,
          ::vkwp::vulkan_raw_resource_or_void ParentT,
          ::vkwp::vulkan_raw_resource_or_void PoolT>
vkwp::Resource<T, ParentT, PoolT>::Resource(Resource && m) noexcept
    : ResourceBase<T>{ std::move(m) }
    , parent_{ m.parent_ }
    , pool_{ m.pool_ }
{
    m.parent_ = nullptr;
    m.pool_   = nullptr;
}

template <::vkwp::vulkan_raw_resource         T,
          ::vkwp::vulkan_raw_resource_or_void ParentT,
          ::vkwp::vulkan_raw_resource_or_void PoolT>
vkwp::Resource<T, ParentT, PoolT>::~Resource() noexcept
{
    this->_reset();
}

template <::vkwp::vulkan_raw_resource         T,
          ::vkwp::vulkan_raw_resource_or_void ParentT,
          ::vkwp::vulkan_raw_resource_or_void PoolT>
void
vkwp::Resource<T, ParentT, PoolT>::reset() & noexcept
{
    this->reset(nullptr, nullptr);
}

template <::vkwp::vulkan_raw_resource         T,
          ::vkwp::vulkan_raw_resource_or_void ParentT,
          ::vkwp::vulkan_raw_resource_or_void PoolT>
T *
vkwp::Resource<T, ParentT, PoolT>::reset(const ParentT & parent, const PoolT & pool) & noexcept
{
    this->_reset();
    this->_ref()  = nullptr;
    this->parent_ = parent;
    this->pool_   = pool;
    return &this->_ref();
}

template <::vkwp::vulkan_raw_resource         T,
          ::vkwp::vulkan_raw_resource_or_void ParentT,
          ::vkwp::vulkan_raw_resource_or_void PoolT>
void
vkwp::Resource<T, ParentT, PoolT>::setExternal(const T & instance) & noexcept
{
    this->_reset();
    this->_ref() = instance;
}

template <::vkwp::vulkan_raw_resource         T,
          ::vkwp::vulkan_raw_resource_or_void ParentT,
          ::vkwp::vulkan_raw_resource_or_void PoolT>
void
vkwp::Resource<T, ParentT, PoolT>::_reset() noexcept
{
    if (this->_ref() != nullptr && this->parent_ != nullptr && this->pool_ != nullptr)
    {
        _deleter(this->_ref(), &this->parent_, &this->pool_);
    }
}

template <::vkwp::vulkan_raw_resource         T,
          ::vkwp::vulkan_raw_resource_or_void ParentT,
          ::vkwp::vulkan_raw_resource_or_void PoolT>
vkwp::Resource<T, ParentT, PoolT> &
vkwp::Resource<T, ParentT, PoolT>::operator=(Resource && rhs) & noexcept
{
    if (this != &rhs)
    {
        this->parent_ = rhs.parent_;
        this->pool_   = rhs.pool_;
        rhs.parent_   = nullptr;
        rhs.pool_     = nullptr;
    }
    ResourceBase<T>::operator=(std::move(rhs));
    return *this;
}

template <::vkwp::vulkan_raw_resource         T,
          ::vkwp::vulkan_raw_resource_or_void ParentT,
          ::vkwp::vulkan_raw_resource_or_void PoolT>
T
vkwp::Resource<T, ParentT, PoolT>::release(ParentT & parent, PoolT & pool) noexcept
{
    pool          = this->pool_;
    parent        = this->parent_;
    auto ret      = this->_ref();
    this->pool_   = nullptr;
    this->parent_ = nullptr;
    this->_ref()  = nullptr;
    return ret;
}

template <::vkwp::vulkan_raw_resource         T,
          ::vkwp::vulkan_raw_resource_or_void ParentT,
          ::vkwp::vulkan_raw_resource_or_void PoolT>
const PoolT &
vkwp::Resource<T, ParentT, PoolT>::_getPool() const noexcept
{
    return this->pool_;
}

template <::vkwp::vulkan_raw_resource         T,
          ::vkwp::vulkan_raw_resource_or_void ParentT,
          ::vkwp::vulkan_raw_resource_or_void PoolT>
const ParentT &
vkwp::Resource<T, ParentT, PoolT>::_getParent() const noexcept
{
    return this->parent_;
}

// ============================================================================================= //
// Resource<T, ParentT, void>
// ============================================================================================= //
template <::vkwp::vulkan_raw_resource T, ::vkwp::vulkan_raw_resource_or_void ParentT>
vkwp::Resource<T, ParentT, void>::Resource()
    : ResourceBase<T>{}
    , parent_{ nullptr }
{}

template <::vkwp::vulkan_raw_resource T, ::vkwp::vulkan_raw_resource_or_void ParentT>
vkwp::Resource<T, ParentT, void>::Resource(const std::string & label)
    : ResourceBase<T>{ label }
    , parent_{ nullptr }
{}

template <::vkwp::vulkan_raw_resource T, ::vkwp::vulkan_raw_resource_or_void ParentT>
vkwp::Resource<T, ParentT, void>::Resource(Resource && m) noexcept
    : ResourceBase<T>{ std::move(m) }
    , parent_{ m.parent_ }
{
    m.parent_ = nullptr;
}

template <::vkwp::vulkan_raw_resource T, ::vkwp::vulkan_raw_resource_or_void ParentT>
vkwp::Resource<T, ParentT, void>::~Resource() noexcept
{
    this->_reset();
}

template <::vkwp::vulkan_raw_resource T, ::vkwp::vulkan_raw_resource_or_void ParentT>
void
vkwp::Resource<T, ParentT, void>::reset() & noexcept
{
    this->reset(nullptr);
}

template <::vkwp::vulkan_raw_resource T, ::vkwp::vulkan_raw_resource_or_void ParentT>
T *
vkwp::Resource<T, ParentT, void>::reset(const ParentT & parent) & noexcept
{
    this->_reset();
    this->_ref()  = nullptr;
    this->parent_ = parent;
    return &this->_ref();
}

template <::vkwp::vulkan_raw_resource T, ::vkwp::vulkan_raw_resource_or_void ParentT>
void
vkwp::Resource<T, ParentT, void>::setExternal(const T & instance) & noexcept
{
    this->_reset();
    this->_ref() = instance;
}

template <::vkwp::vulkan_raw_resource T, ::vkwp::vulkan_raw_resource_or_void ParentT>
void
vkwp::Resource<T, ParentT, void>::_reset() noexcept
{
    if (this->_ref() != nullptr && this->parent_ != nullptr)
    {
        _deleter(this->_ref(), &this->parent_, nullptr);
    }
}

template <::vkwp::vulkan_raw_resource T, ::vkwp::vulkan_raw_resource_or_void ParentT>
vkwp::Resource<T, ParentT, void> &
vkwp::Resource<T, ParentT, void>::operator=(Resource && rhs) & noexcept
{
    if (this != &rhs)
    {
        this->parent_ = rhs.parent_;
        rhs.parent_   = nullptr;
    }
    ResourceBase<T>::operator=(std::move(rhs));
    return *this;
}

template <::vkwp::vulkan_raw_resource T, ::vkwp::vulkan_raw_resource_or_void ParentT>
T
vkwp::Resource<T, ParentT, void>::release(ParentT & parent) noexcept
{
    parent        = this->parent_;
    auto ret      = this->_ref();
    this->parent_ = nullptr;
    this->_ref()  = nullptr;
    return ret;
}

template <::vkwp::vulkan_raw_resource T, ::vkwp::vulkan_raw_resource_or_void ParentT>
const ParentT &
vkwp::Resource<T, ParentT, void>::_getParent() const noexcept
{
    return this->parent_;
}

// ============================================================================================= //
// Resource<T, void, void>
// ============================================================================================= //
template <::vkwp::vulkan_raw_resource T>
vkwp::Resource<T, void, void>::Resource()
    : ResourceBase<T>{}
    , is_external_{ false }
{}

template <::vkwp::vulkan_raw_resource T>
vkwp::Resource<T, void, void>::Resource(const std::string & label)
    : ResourceBase<T>{ label }
    , is_external_{ false }
{}

template <::vkwp::vulkan_raw_resource T>
vkwp::Resource<T, void, void>::Resource(Resource && m) noexcept
    : ResourceBase<T>{ std::move(m) }
    , is_external_{ m.is_external_ }
{}

template <::vkwp::vulkan_raw_resource T>
vkwp::Resource<T, void, void>::~Resource() noexcept
{
    this->_reset();
}

template <::vkwp::vulkan_raw_resource T>
T *
vkwp::Resource<T, void, void>::reset() & noexcept
{
    this->_reset();
    this->_ref()       = nullptr;
    this->is_external_ = false;
    return &this->_ref();
}

template <::vkwp::vulkan_raw_resource T>
void
vkwp::Resource<T, void, void>::setExternal(const T & instance) & noexcept
{
    this->_reset();
    this->_ref()       = instance;
    this->is_external_ = true;
}

template <::vkwp::vulkan_raw_resource T>
void
vkwp::Resource<T, void, void>::_reset() noexcept
{
    if (this->_ref() != nullptr && !this->is_external_)
    {
        _deleter(this->_ref(), nullptr, nullptr);
    }
}

template <::vkwp::vulkan_raw_resource T>
vkwp::Resource<T, void, void> &
vkwp::Resource<T, void, void>::operator=(Resource && rhs) & noexcept
{
    ResourceBase<T>::operator=(std::move(rhs));
    return *this;
}

template <::vkwp::vulkan_raw_resource T>
T
vkwp::Resource<T, void, void>::release() noexcept
{
    auto ret     = this->_ref();
    this->_ref() = nullptr;
    return ret;
}

#define SFL_EXPLICIT_DEFINE(VkType, VkParentType, VkPoolType) \
    template class vkwp::ResourceBase<VkType>;         \
    template class vkwp::Resource<VkType, VkParentType, VkPoolType>;

SFL_EXPLICIT_DEFINE(VkInstance, void, void)
SFL_EXPLICIT_DEFINE(VkPhysicalDevice, void, void)
SFL_EXPLICIT_DEFINE(VkDevice, void, void)
SFL_EXPLICIT_DEFINE(VkQueue, void, void)
SFL_EXPLICIT_DEFINE(VkImage, VkDevice, void)
SFL_EXPLICIT_DEFINE(VkImageView, VkDevice, void)
SFL_EXPLICIT_DEFINE(VkDeviceMemory, VkDevice, void)
SFL_EXPLICIT_DEFINE(VkSurfaceKHR, VkInstance, void)
SFL_EXPLICIT_DEFINE(VkSwapchainKHR, VkDevice, void)
SFL_EXPLICIT_DEFINE(VkDescriptorPool, VkDevice, void)
SFL_EXPLICIT_DEFINE(VkDescriptorSetLayout, VkDevice, void)
SFL_EXPLICIT_DEFINE(VkDescriptorSet, VkDevice, VkDescriptorPool)
SFL_EXPLICIT_DEFINE(VkPipelineLayout, VkDevice, void)
SFL_EXPLICIT_DEFINE(VkPipeline, VkDevice, void)
SFL_EXPLICIT_DEFINE(VkRenderPass, VkDevice, void)
SFL_EXPLICIT_DEFINE(VkFramebuffer, VkDevice, void)
SFL_EXPLICIT_DEFINE(VkSampler, VkDevice, void)
SFL_EXPLICIT_DEFINE(VkShaderModule, VkDevice, void)
SFL_EXPLICIT_DEFINE(VkCommandPool, VkDevice, void)
SFL_EXPLICIT_DEFINE(VkCommandBuffer, VkDevice, VkCommandPool)
SFL_EXPLICIT_DEFINE(VkBuffer, VkDevice, void)
SFL_EXPLICIT_DEFINE(VkFence, VkDevice, void)
SFL_EXPLICIT_DEFINE(VkSemaphore, VkDevice, void)

#undef SFL_EXPLICIT_DEFINE