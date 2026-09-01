#include "vkwp/command_buffer.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"

#include "vkwp/command_pool.hpp"
#include "vkwp/mutex.hpp"
#include "vkwp/vulkan_error.hpp"
#include "vkwp/vulkan_functions.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete(VkCommandBuffer                             i,
                    PFN_vkFreeCommandBuffers                    fn,
                    const std::tuple<VkDevice, VkCommandPool> & args) noexcept
{
    if (std::get<1>(args) != nullptr)
    {
        // VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT がセットされた CommandPool の場合だけ呼び出す必要がある.
        fn(std::get<0>(args), std::get<1>(args), 1, &i);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CommandBuffer class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
std::vector<vkwp::CommandBuffer>
vkwp::CommandBuffer::allocateCommandBuffers(const CommandPool &        command_pool,
                                            const VkCommandBufferLevel level,
                                            std::vector<std::string>   command_buffer_labels)
{
    return CommandBuffer::allocateCommandBuffers(command_pool,  //
                                                 level,
                                                 std::move(command_buffer_labels),
                                                 {});
}

/* STATIC */
std::vector<vkwp::CommandBuffer>
vkwp::CommandBuffer::allocateCommandBuffers(const CommandPool &          command_pool,
                                            const VkCommandBufferLevel   level,
                                            std::vector<std::string>     command_buffer_labels,
                                            const OptionalAllocateArgs & opt_args)
{
    VKWP_VALIDATE_ARG(command_pool);

    std::vector<CommandBuffer> ret;
    ret.reserve(command_buffer_labels.size());
    for (auto & label : command_buffer_labels)
    {
        ret.emplace_back(CommandBuffer{ std::move(label) });
    }

    std::vector<VkCommandBuffer>      cmdbufs(command_buffer_labels.size(), nullptr);
    const VkCommandBufferAllocateInfo ai{
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = opt_args.p_next,
        .commandPool        = command_pool,
        .level              = level,
        .commandBufferCount = static_cast<std::uint32_t>(command_buffer_labels.size()),
    };

    const auto device          = std::get<0>(command_pool.parents_);
    const auto require_to_free = (command_pool.getCreateFlags() & VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT) != 0;
    auto * const allocate_fn
        = (opt_args.functions != nullptr && opt_args.functions->vkAllocateCommandBuffers != nullptr)
            ? opt_args.functions->vkAllocateCommandBuffers
            : vkAllocateCommandBuffers;
    auto * const free_fn = (opt_args.functions != nullptr && opt_args.functions->vkFreeCommandBuffers != nullptr)
                             ? opt_args.functions->vkFreeCommandBuffers
                             : vkFreeCommandBuffers;

    {
        // lock は destructor が noexcept 保証できないので,
        // reset() しきってから unlock (scope out) する.
        LockGuardOpt lock{ command_pool.mutex_.get() };

        VulkanError::check(  //
            allocate_fn(device, &ai, cmdbufs.data()),
            "Failed to vkAllocateCommandBuffers().");

        for (std::size_t i = 0; i < cmdbufs.size(); ++i)
        {
            *ret[i]._reset(free_fn,
                           device,
                           // vkFreeCommandBuffers を呼ばなくて良い場合は cmdpool に null を渡す
                           require_to_free  //
                               ? static_cast<VkCommandPool>(command_pool)
                               : nullptr)
                = cmdbufs[i];

            ret[i].mutex_ = command_pool.mutex_.get();
        }
    }

    if (opt_args.debug_utils != nullptr)
    {
        for (const auto & cmdbuf : ret)
        {
            opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                             VK_OBJECT_TYPE_COMMAND_BUFFER,
                                                             static_cast<VkCommandBuffer>(cmdbuf),
                                                             cmdbuf.getLabel());
        }
    }
    if (opt_args.functions != nullptr)
    {
        for (auto & cmdbuf : ret)
        {
            cmdbuf._vkBeginCommandBuffer = (opt_args.functions->vkBeginCommandBuffer != nullptr)
                                             ? opt_args.functions->vkBeginCommandBuffer
                                             : vkBeginCommandBuffer;
            cmdbuf._vkEndCommandBuffer   = (opt_args.functions->vkEndCommandBuffer != nullptr)
                                             ? opt_args.functions->vkEndCommandBuffer
                                             : vkEndCommandBuffer;
        }
    }
    else
    {
        for (auto & cmdbuf : ret)
        {
            cmdbuf._vkBeginCommandBuffer = vkBeginCommandBuffer;
            cmdbuf._vkEndCommandBuffer   = vkEndCommandBuffer;
        }
    }

    return ret;
}

/* STATIC */
vkwp::CommandBuffer
vkwp::CommandBuffer::allocateCommandBuffer(std::string                label,
                                           const CommandPool &        command_pool,
                                           const VkCommandBufferLevel level)
{
    return CommandBuffer::allocateCommandBuffer(std::move(label),  //
                                                command_pool,
                                                level,
                                                {});
}

/* STATIC */
vkwp::CommandBuffer
vkwp::CommandBuffer::allocateCommandBuffer(std::string                  label,
                                           const CommandPool &          command_pool,
                                           const VkCommandBufferLevel   level,
                                           const OptionalAllocateArgs & opt_args)
{
    CommandBuffer                     ret{ std::move(label) };
    const VkCommandBufferAllocateInfo ai{
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = opt_args.p_next,
        .commandPool        = command_pool,
        .level              = level,
        .commandBufferCount = 1,
    };

    // lock は destructor が noexcept 保証できないので,
    // reset() しきってから unlock (scope out) する.
    LockGuardOpt lock{ command_pool.mutex_.get() };

    auto       device          = std::get<0>(command_pool.parents_);
    const auto require_to_free = (command_pool.getCreateFlags() & VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT) != 0;
    auto * const allocate_fn
        = (opt_args.functions != nullptr && opt_args.functions->vkAllocateCommandBuffers != nullptr)
            ? opt_args.functions->vkAllocateCommandBuffers
            : vkAllocateCommandBuffers;
    auto * const free_fn = (opt_args.functions != nullptr && opt_args.functions->vkFreeCommandBuffers != nullptr)
                             ? opt_args.functions->vkFreeCommandBuffers
                             : vkFreeCommandBuffers;

    VulkanError::check(  //
        allocate_fn(     //
            device,
            &ai,
            ret._reset(free_fn,
                       device,
                       // vkFreeCommandBuffers を呼ばなくて良い場合は cmdpool に null を渡す
                       require_to_free  //
                           ? static_cast<VkCommandPool>(command_pool)
                           : nullptr)),
        "Failed to vkAllocateCommandBuffers(). (single allocation)");

    ret.mutex_ = command_pool.mutex_.get();

    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_COMMAND_BUFFER,
                                                         static_cast<VkCommandBuffer>(ret),
                                                         ret.getLabel());
    }
    if (opt_args.functions != nullptr)
    {
        ret._vkBeginCommandBuffer = (opt_args.functions->vkBeginCommandBuffer != nullptr)
                                      ? opt_args.functions->vkBeginCommandBuffer
                                      : vkBeginCommandBuffer;
        ret._vkEndCommandBuffer   = (opt_args.functions->vkEndCommandBuffer != nullptr)
                                      ? opt_args.functions->vkEndCommandBuffer
                                      : vkEndCommandBuffer;
    }
    else
    {
        ret._vkBeginCommandBuffer = vkBeginCommandBuffer;
        ret._vkEndCommandBuffer   = vkEndCommandBuffer;
    }
    return ret;
}

/* STATIC */
vkwp::CommandBuffer
vkwp::CommandBuffer::allocatePrimaryCommandBuffer(std::string label, const CommandPool & command_pool)
{
    return CommandBuffer::allocatePrimaryCommandBuffer(std::move(label), command_pool, {});
}

/* STATIC */
vkwp::CommandBuffer
vkwp::CommandBuffer::allocatePrimaryCommandBuffer(std::string                  label,
                                                  const CommandPool &          command_pool,
                                                  const OptionalAllocateArgs & opt_args)
{
    return allocateCommandBuffer(std::move(label), command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, opt_args);
}

vkwp::CommandBuffer::CommandBuffer()
    : Resource{}
    , mutex_{ nullptr }
{}

vkwp::CommandBuffer::CommandBuffer(std::string && label)
    : Resource{ std::move(label) }
    , mutex_{ nullptr }
{}

vkwp::CommandBuffer::CommandBuffer(CommandBuffer && m) noexcept
    : Resource{ std::move(m) }
    , mutex_{ m.mutex_ }
    , _vkBeginCommandBuffer{ m._vkBeginCommandBuffer }
    , _vkEndCommandBuffer{ m._vkEndCommandBuffer }
{
    if (this != &m)
    {
        m.mutex_                = nullptr;
        m._vkBeginCommandBuffer = nullptr;
        m._vkEndCommandBuffer   = nullptr;
    }
}

vkwp::CommandBuffer::~CommandBuffer() noexcept
{
    if (*this && std::get<1>(this->parents_) != nullptr)
    {
        LockGuardOpt lock{ this->mutex_ };
        this->reset();
    }
}

vkwp::CommandBuffer &
vkwp::CommandBuffer::operator=(CommandBuffer && rhs) & noexcept
{
    if (this != &rhs)
    {
        *static_cast<Resource *>(this) = std::move(rhs);
        this->mutex_                   = rhs.mutex_;
        this->_vkBeginCommandBuffer    = rhs._vkBeginCommandBuffer;
        this->_vkEndCommandBuffer      = rhs._vkEndCommandBuffer;
        rhs.mutex_                     = nullptr;
        rhs._vkBeginCommandBuffer      = nullptr;
        rhs._vkEndCommandBuffer        = nullptr;
    }
    return *this;
}

void
vkwp::CommandBuffer::beginCommandBuffer(const VkCommandBufferUsageFlags flags, const void * const p_next) const
{
    this->_beginCommandBuffer(nullptr, flags, p_next);
}

void
vkwp::CommandBuffer::beginCommandBuffer(const VkCommandBufferInheritanceInfo & inheritance_info,
                                        const VkCommandBufferUsageFlags        flags,
                                        const void * const                     p_next) const
{
    this->_beginCommandBuffer(&inheritance_info, flags, p_next);
}

void
vkwp::CommandBuffer::_beginCommandBuffer(const VkCommandBufferInheritanceInfo * inheritance_info,
                                         const VkCommandBufferUsageFlags        flags,
                                         const void * const                     p_next) const
{
    const VkCommandBufferBeginInfo bi{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = p_next,
        .flags            = flags,
        .pInheritanceInfo = inheritance_info,
    };
    VulkanError::check(this->_vkBeginCommandBuffer(*this, &bi), "Failed to vkBeginCommandBuffer().");
}

void
vkwp::CommandBuffer::endCommandBuffer() const
{
    VulkanError::check(this->_vkEndCommandBuffer(*this), "Failed to vkEndCommandBuffer().");
}

void
vkwp::CommandBuffer::lock() const
{
    if (this->mutex_ != nullptr)
    {
        this->mutex_->lock();
    }
}

bool
vkwp::CommandBuffer::try_lock() const
{
    if (this->mutex_ != nullptr)
    {
        return this->mutex_->tryLock();
    }
    else
    {
        return true;
    }
}

void
vkwp::CommandBuffer::unlock() const
{
    if (this->mutex_ != nullptr)
    {
        this->mutex_->unlock();
    }
}
