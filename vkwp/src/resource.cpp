#include "vkwp/resource.hpp"

#include "impl/deleter.hpp"
#include "vkwp/vulkan.hpp"

template <::vkwp::vulkan_raw_resource T, typename... ParentTs>
vkwp::Resource<T, ParentTs...>::Resource()
    : label_{}
    , external_{ false }
    , instance_{ nullptr }
    , parents_{}
{}

template <::vkwp::vulkan_raw_resource T, typename... ParentTs>
vkwp::Resource<T, ParentTs...>::Resource(const std::string & label)
    : label_{ label }
    , external_{ false }
    , instance_{ nullptr }
    , parents_{}
{}

template <::vkwp::vulkan_raw_resource T, typename... ParentTs>
vkwp::Resource<T, ParentTs...>::Resource(const std::string & label, const T & external)
    : label_{ label }
    , external_{ true }
    , instance_{ external }
    , parents_{}
{}

template <::vkwp::vulkan_raw_resource T, typename... ParentTs>
vkwp::Resource<T, ParentTs...>::Resource(Resource && m) noexcept
    : label_{ std::move(m.label_) }
    , external_{ m.external_ }
    , instance_{ m.instance_ }
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
        this->label_    = std::move(rhs.label_);
        this->external_ = rhs.external_;
        this->instance_ = rhs.instance_;
        this->parents_  = std::move(rhs.parents_);
        rhs.external_   = false;
        rhs.instance_   = nullptr;
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
vkwp::Resource<T, ParentTs...>::_reset(ParentTs... parents...) & noexcept
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
template class vkwp::Resource<VkDevice>;
template class vkwp::Resource<VkDeviceMemory, VkDevice>;
template class vkwp::Resource<VkInstance>;
template class vkwp::Resource<VkPhysicalDevice>;
template class vkwp::Resource<VkQueue>;

#if 0
#    include "impl/assert.hpp"
#    include "vkwp/vulkan.hpp"
#    include "vkwp/vulkan_error.hpp"

namespace {

#    define VKWP_DEFINE_DELETER(VkType, VkParentType, VkPoolType) \
        void _deleter(const VkType & i, const VkParentType * pa, const VkPoolType * po) noexcept

// clang-format off
VKWP_DEFINE_DELETER(VkInstance,                 void,       void)             { vkDestroyInstance(i, nullptr); }
VKWP_DEFINE_DELETER(VkPhysicalDevice,           void,       void)             {}
VKWP_DEFINE_DELETER(VkDevice,                   void,       void)             { vkDestroyDevice(i, nullptr); }
VKWP_DEFINE_DELETER(VkQueue,                    VkDevice,   void)             {}
VKWP_DEFINE_DELETER(VkImage,                    VkDevice,   void)             { vkDestroyImage(*pa, i, nullptr); }
VKWP_DEFINE_DELETER(VkImageView,                VkDevice,   void)             { vkDestroyImageView(*pa, i, nullptr); }
VKWP_DEFINE_DELETER(VkDeviceMemory,             VkDevice,   void)             { vkFreeMemory(*pa, i, nullptr); }
VKWP_DEFINE_DELETER(VkSurfaceKHR,               VkInstance, void)             { vkDestroySurfaceKHR(*pa, i, nullptr); }
VKWP_DEFINE_DELETER(VkSwapchainKHR,             VkDevice,   void)             { vkDestroySwapchainKHR(*pa, i, nullptr); }
VKWP_DEFINE_DELETER(VkDescriptorPool,           VkDevice,   void)             { vkDestroyDescriptorPool(*pa, i, nullptr); }
VKWP_DEFINE_DELETER(VkDescriptorSetLayout,      VkDevice,   void)             { vkDestroyDescriptorSetLayout(*pa, i, nullptr); }
VKWP_DEFINE_DELETER(VkDescriptorSet,            VkDevice,   VkDescriptorPool) { vkFreeDescriptorSets(*pa, *po, 1, &i); }
VKWP_DEFINE_DELETER(VkPipelineLayout,           VkDevice,   void)             { vkDestroyPipelineLayout(*pa, i, nullptr); }
VKWP_DEFINE_DELETER(VkPipeline,                 VkDevice,   void)             { vkDestroyPipeline(*pa, i, nullptr); }
VKWP_DEFINE_DELETER(VkRenderPass,               VkDevice,   void)             { vkDestroyRenderPass(*pa, i, nullptr); }
VKWP_DEFINE_DELETER(VkFramebuffer,              VkDevice,   void)             { vkDestroyFramebuffer(*pa, i, nullptr); }
VKWP_DEFINE_DELETER(VkSampler,                  VkDevice,   void)             { vkDestroySampler(*pa, i, nullptr); }
VKWP_DEFINE_DELETER(VkShaderModule,             VkDevice,   void)             { vkDestroyShaderModule(*pa, i, nullptr); }
VKWP_DEFINE_DELETER(VkCommandPool,              VkDevice,   void)             { vkDestroyCommandPool(*pa, i, nullptr); }
VKWP_DEFINE_DELETER(VkCommandBuffer,            VkDevice,   VkCommandPool)    { vkFreeCommandBuffers(*pa, *po, 1, &i); }
VKWP_DEFINE_DELETER(VkBuffer,                   VkDevice,   void)             { vkDestroyBuffer(*pa, i, nullptr); }
VKWP_DEFINE_DELETER(VkFence,                    VkDevice,   void)             { vkDestroyFence(*pa, i, nullptr); }
VKWP_DEFINE_DELETER(VkSemaphore,                VkDevice,   void)             { vkDestroySemaphore(*pa, i, nullptr); }
VKWP_DEFINE_DELETER(VkAccelerationStructureKHR, VkDevice,   void)             { vkDestroyAccelerationStructureKHR(*pa, i, nullptr); }
// clang-format on

#    undef DEFINE_DELETER

}  // namespace

template <typename T>
vkwp::ResourceBase<T>::ResourceBase()
    : label_{}
    , instance_{ VK_NULL_HANDLE }
    , external_{ false }
{}

template <typename T>
vkwp::ResourceBase<T>::ResourceBase(const std::string & label)
    : label_{ label }
    , instance_{ VK_NULL_HANDLE }
    , external_{ false }
{}

template <typename T>
vkwp::ResourceBase<T>::ResourceBase(ResourceBase<T> && m) noexcept
    : label_{ std::move(m.label_) }
    , instance_{ m.instance_ }
    , external_{ m.external_ }
{
    if (this != &m)
    {
        m.instance_ = VK_NULL_HANDLE;
        m.external_ = false;
    }
}

template <typename T>
vkwp::ResourceBase<T>::~ResourceBase() noexcept = default;

template <typename T>
vkwp::ResourceBase<T> &
vkwp::ResourceBase<T>::operator=(ResourceBase<T> && rhs) & noexcept
{
    static_assert(std::is_nothrow_move_assignable_v<std::string>);
    static_assert(std::is_nothrow_copy_assignable_v<T>);
    if (this != &rhs)
    {
        this->label_    = std::move(rhs.label_);
        this->instance_ = rhs.instance_;
        this->external_ = rhs.external_;
        rhs.instance_   = nullptr;
        rhs.external_   = false;
    }
    return *this;
}

template <typename T>
bool
vkwp::ResourceBase<T>::operator!() const & noexcept
{
    return this->instance_ == nullptr;
}

template <typename T>
vkwp::ResourceBase<T>::operator bool() const & noexcept
{
    return this->instance_ != nullptr;
}

template <typename T>
vkwp::ResourceBase<T>::operator T() const & noexcept
{
    static_assert(std::is_nothrow_copy_assignable_v<T>);
    return this->instance_;
}

template <typename T>
const T &
vkwp::ResourceBase<T>::operator*() const & noexcept
{
    return this->instance_;
}

template <typename T>
const T *
vkwp::ResourceBase<T>::ptr() const & noexcept
{
    return &this->instance_;
}

template <typename T>
const std::string &
vkwp::ResourceBase<T>::getLabel() const & noexcept
{
    return this->label_;
}

template <typename T>
bool
vkwp::ResourceBase<T>::isExternal() const & noexcept
{
    return this->external_;
}

template <typename T>
const T &
vkwp::ResourceBase<T>::_ref() const noexcept
{
    return this->instance_;
}

template <typename T>
T &
vkwp::ResourceBase<T>::_ref() noexcept
{
    return this->instance_;
}

// ============================================================================================= //
// Resource<T, ParentT, PoolT>
// ============================================================================================= //
template <typename         T,
          typename_or_void ParentT,
          typename_or_void PoolT>
vkwp::Resource<T, ParentT, PoolT>::Resource()
    : ResourceBase<T>{}
    , parent_{ nullptr }
    , pool_{ nullptr }
{}

template <typename         T,
          typename_or_void ParentT,
          typename_or_void PoolT>
vkwp::Resource<T, ParentT, PoolT>::Resource(const std::string & label)
    : ResourceBase<T>{ label }
    , parent_{ nullptr }
    , pool_{ nullptr }
{}

template <typename         T,
          typename_or_void ParentT,
          typename_or_void PoolT>
vkwp::Resource<T, ParentT, PoolT>::Resource(Resource && m) noexcept
    : ResourceBase<T>{ std::move(m) }
    , parent_{ m.parent_ }
    , pool_{ m.pool_ }
{
    if (this != &m)
    {
        m.parent_ = nullptr;
        m.pool_   = nullptr;
    }
}

template <typename         T,
          typename_or_void ParentT,
          typename_or_void PoolT>
vkwp::Resource<T, ParentT, PoolT>::~Resource() noexcept
{
    this->_reset();
}

template <typename         T,
          typename_or_void ParentT,
          typename_or_void PoolT>
void
vkwp::Resource<T, ParentT, PoolT>::reset() & noexcept
{
    this->reset(nullptr, nullptr);
}


template <typename         T,
          typename_or_void ParentT,
          typename_or_void PoolT>
T *
vkwp::Resource<T, ParentT, PoolT>::reset(const ParentT & parent, const PoolT & pool) & noexcept
{
    this->_reset();
    this->_ref()    = nullptr;
    this->parent_   = parent;
    this->pool_     = pool;
    this->external_ = false;
    return &this->_ref();
}

template <typename         T,
          typename_or_void ParentT,
          typename_or_void PoolT>
void
vkwp::Resource<T, ParentT, PoolT>::setExternal(const T & instance, const ParentT & parent) & noexcept
{
    this->_reset();
    this->_ref()    = instance;
    this->parent_   = parent;
    this->external_ = true;
}

template <typename         T,
          typename_or_void ParentT,
          typename_or_void PoolT>
void
vkwp::Resource<T, ParentT, PoolT>::_reset() noexcept
{
    if (!this->external_ && this->_ref() != nullptr)
    {
        _deleter(this->_ref(), &this->parent_, &this->pool_);
    }
}

template <typename         T,
          typename_or_void ParentT,
          typename_or_void PoolT>
vkwp::Resource<T, ParentT, PoolT> &
vkwp::Resource<T, ParentT, PoolT>::operator=(Resource && rhs) & noexcept
{
    if (this != &rhs)
    {
        this->_reset();
        this->parent_   = rhs.parent_;
        this->pool_     = rhs.pool_;
        this->external_ = rhs.external_;
        rhs.parent_     = nullptr;
        rhs.pool_       = nullptr;
        rhs.external_   = false;
    }
    ResourceBase<T>::operator=(std::move(rhs));
    return *this;
}

template <typename         T,
          typename_or_void ParentT,
          typename_or_void PoolT>
T
vkwp::Resource<T, ParentT, PoolT>::release(ParentT & parent, PoolT & pool, bool * const external) noexcept
{
    pool     = this->pool_;
    parent   = this->parent_;
    auto ret = this->_ref();
    if (external != nullptr)
    {
        *external = this->external_;
    }
    this->pool_     = nullptr;
    this->parent_   = nullptr;
    this->_ref()    = nullptr;
    this->external_ = false;
    return ret;
}

template <typename         T,
          typename_or_void ParentT,
          typename_or_void PoolT>
const PoolT &
vkwp::Resource<T, ParentT, PoolT>::_getPool() const noexcept
{
    return this->pool_;
}

template <typename         T,
          typename_or_void ParentT,
          typename_or_void PoolT>
const ParentT &
vkwp::Resource<T, ParentT, PoolT>::_getParent() const noexcept
{
    return this->parent_;
}

// ============================================================================================= //
// Resource<T, ParentT, void>
// ============================================================================================= //
template <typename T, typename_or_void ParentT>
vkwp::Resource<T, ParentT, void>::Resource()
    : ResourceBase<T>{}
    , parent_{ nullptr }
{}

template <typename T, typename_or_void ParentT>
vkwp::Resource<T, ParentT, void>::Resource(const std::string & label)
    : ResourceBase<T>{ label }
    , parent_{ nullptr }
{}

template <typename T, typename_or_void ParentT>
vkwp::Resource<T, ParentT, void>::Resource(Resource && m) noexcept
    : ResourceBase<T>{ std::move(m) }
    , parent_{ m.parent_ }
{
    if (this != &m)
    {
        m.parent_ = nullptr;
    }
}

template <typename T, typename_or_void ParentT>
vkwp::Resource<T, ParentT, void>::~Resource() noexcept
{
    this->_reset();
}

template <typename T, typename_or_void ParentT>
void
vkwp::Resource<T, ParentT, void>::reset() & noexcept
{
    this->reset(nullptr);
}

template <typename T, typename_or_void ParentT>
T *
vkwp::Resource<T, ParentT, void>::reset(const ParentT & parent) & noexcept
{
    this->_reset();
    this->_ref()    = nullptr;
    this->parent_   = parent;
    this->external_ = false;
    return &this->_ref();
}

template <typename T, typename_or_void ParentT>
void
vkwp::Resource<T, ParentT, void>::setExternal(const T & instance, const ParentT & parent) & noexcept
{
    this->_reset();
    this->_ref()    = instance;
    this->parent_   = parent;
    this->external_ = true;
}

template <typename T, typename_or_void ParentT>
void
vkwp::Resource<T, ParentT, void>::_reset() noexcept
{
    if (!this->external_ && this->_ref() != nullptr)
    {
        _deleter(this->_ref(), &this->parent_, nullptr);
    }
}

template <typename T, typename_or_void ParentT>
vkwp::Resource<T, ParentT, void> &
vkwp::Resource<T, ParentT, void>::operator=(Resource && rhs) & noexcept
{
    if (this != &rhs)
    {
        this->_reset();
        this->parent_   = rhs.parent_;
        this->external_ = rhs.external_;
        rhs.parent_     = nullptr;
        rhs.external_   = false;
    }
    ResourceBase<T>::operator=(std::move(rhs));
    return *this;
}

template <typename T, typename_or_void ParentT>
T
vkwp::Resource<T, ParentT, void>::release(ParentT & parent, bool * const external) noexcept
{
    auto ret = this->_ref();
    parent   = this->parent_;
    if (external != nullptr)
    {
        *external = this->external_;
    }
    this->_ref()    = nullptr;
    this->parent_   = nullptr;
    this->external_ = false;
    return ret;
}

template <typename T, typename_or_void ParentT>
const ParentT &
vkwp::Resource<T, ParentT, void>::_getParent() const noexcept
{
    return this->parent_;
}

// ============================================================================================= //
// Resource<T, void, void>
// ============================================================================================= //
template <typename T>
vkwp::Resource<T, void, void>::Resource()
    : ResourceBase<T>{}
{}

template <typename T>
vkwp::Resource<T, void, void>::Resource(const std::string & label)
    : ResourceBase<T>{ label }
{}

template <typename T>
vkwp::Resource<T, void, void>::Resource(Resource && m) noexcept
    : ResourceBase<T>{ std::move(m) }
{}

template <typename T>
vkwp::Resource<T, void, void>::~Resource() noexcept
{
    this->_reset();
}

template <typename T>
T *
vkwp::Resource<T, void, void>::reset() & noexcept
{
    this->_reset();
    this->_ref()    = nullptr;
    this->external_ = false;
    return &this->_ref();
}

template <typename T>
void
vkwp::Resource<T, void, void>::setExternal(const T & instance) & noexcept
{
    this->_reset();
    this->_ref()    = instance;
    this->external_ = true;
}

template <typename T>
void
vkwp::Resource<T, void, void>::_reset() noexcept
{
    if (!this->external_ && this->_ref() != nullptr)
    {
        _deleter(this->_ref(), nullptr, nullptr);
    }
}

template <typename T>
vkwp::Resource<T, void, void> &
vkwp::Resource<T, void, void>::operator=(Resource && rhs) & noexcept
{
    if (this != &rhs)
    {
        this->_reset();
    }
    ResourceBase<T>::operator=(std::move(rhs));
    return *this;
}

template <typename T>
T
vkwp::Resource<T, void, void>::release(bool * const external) noexcept
{
    auto ret = this->_ref();
    if (external != nullptr)
    {
        *external = this->external_;
    }
    this->_ref()    = nullptr;
    this->external_ = false;
    return ret;
}

#    define VKWP_EXPLICIT_DEFINE(VkType, VkParentType, VkPoolType) \
        template class vkwp::ResourceBase<VkType>;                 \
        template class vkwp::Resource<VkType, VkParentType, VkPoolType>;

VKWP_EXPLICIT_DEFINE(VkInstance, void, void)
VKWP_EXPLICIT_DEFINE(VkPhysicalDevice, void, void)
VKWP_EXPLICIT_DEFINE(VkDevice, void, void)
VKWP_EXPLICIT_DEFINE(VkQueue, VkDevice, void)
VKWP_EXPLICIT_DEFINE(VkImage, VkDevice, void)
VKWP_EXPLICIT_DEFINE(VkImageView, VkDevice, void)
VKWP_EXPLICIT_DEFINE(VkDeviceMemory, VkDevice, void)
VKWP_EXPLICIT_DEFINE(VkSurfaceKHR, VkInstance, void)
VKWP_EXPLICIT_DEFINE(VkSwapchainKHR, VkDevice, void)
VKWP_EXPLICIT_DEFINE(VkDescriptorPool, VkDevice, void)
VKWP_EXPLICIT_DEFINE(VkDescriptorSetLayout, VkDevice, void)
VKWP_EXPLICIT_DEFINE(VkDescriptorSet, VkDevice, VkDescriptorPool)
VKWP_EXPLICIT_DEFINE(VkPipelineLayout, VkDevice, void)
VKWP_EXPLICIT_DEFINE(VkPipeline, VkDevice, void)
VKWP_EXPLICIT_DEFINE(VkRenderPass, VkDevice, void)
VKWP_EXPLICIT_DEFINE(VkFramebuffer, VkDevice, void)
VKWP_EXPLICIT_DEFINE(VkSampler, VkDevice, void)
VKWP_EXPLICIT_DEFINE(VkShaderModule, VkDevice, void)
VKWP_EXPLICIT_DEFINE(VkCommandPool, VkDevice, void)
VKWP_EXPLICIT_DEFINE(VkCommandBuffer, VkDevice, VkCommandPool)
VKWP_EXPLICIT_DEFINE(VkBuffer, VkDevice, void)
VKWP_EXPLICIT_DEFINE(VkFence, VkDevice, void)
VKWP_EXPLICIT_DEFINE(VkSemaphore, VkDevice, void)

#    undef VKWP_EXPLICIT_DEFINE
#endif