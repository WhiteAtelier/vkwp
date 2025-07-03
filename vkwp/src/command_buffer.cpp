#include "vkwp/command_buffer.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"
#include "vkwp/command_pool.hpp"
#include "vkwp/mutex.hpp"
#include "vkwp/vulkan_error.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete<VkCommandBuffer, VkDevice, VkCommandPool>(VkCommandBuffer                             i,
                                                              const std::tuple<VkDevice, VkCommandPool> & args) noexcept
{
    if (std::get<1>(args) != nullptr)
    {
        // VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT がセットされた CommandPool の場合だけ呼び出す必要がある.
        vkFreeCommandBuffers(std::get<0>(args), std::get<1>(args), 1, &i);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CommandBuffer class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
std::vector<vkwp::CommandBuffer>
vkwp::CommandBuffer::allocateCommandBuffers(const CommandPool &              command_pool,
                                            const VkCommandBufferLevel       level,
                                            const std::vector<std::string> & command_buffer_labels,
                                            const OptionalAllocateArgs &     opt_args)
{
    VKWP_VALIDATE_ARG(command_pool);

    std::vector<CommandBuffer> ret;
    ret.reserve(command_buffer_labels.size());
    for (const auto & label : command_buffer_labels)
    {
        ret.emplace_back(CommandBuffer{ label });
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

    {
        // lock は destructor が noexcept 保証できないので,
        // reset() しきってから unlock (scope out) する.
        LockGuardOpt lock{ command_pool.mutex_.get() };

        VulkanError::check(  //
            vkAllocateCommandBuffers(device, &ai, cmdbufs.data()),
            "Failed to vkAllocateCommandBuffers().");

        for (std::size_t i = 0; i < cmdbufs.size(); ++i)
        {
            *ret[i]._reset(device,
                           // vkFreeCommandBuffers を呼ばなくて良い場合は cmdpool に null を渡す
                           require_to_free ? command_pool : nullptr)
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

    return ret;
}

/* STATIC */
vkwp::CommandBuffer
vkwp::CommandBuffer::allocateCommandBuffer(const std::string &          label,
                                           const CommandPool &          command_pool,
                                           const VkCommandBufferLevel   level,
                                           const OptionalAllocateArgs & opt_args)
{
    CommandBuffer                     ret{ label };
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

    VulkanError::check(            //
        vkAllocateCommandBuffers(  //
            device,
            &ai,
            ret._reset(device,
                       // vkFreeCommandBuffers を呼ばなくて良い場合は cmdpool に null を渡す
                       require_to_free ? command_pool : nullptr)),
        "Failed to vkAllocateCommandBuffers(). (single allocation)");

    ret.mutex_ = command_pool.mutex_.get();

    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_COMMAND_BUFFER,
                                                         static_cast<VkCommandBuffer>(ret),
                                                         ret.getLabel());
    }
    return ret;
}

/* STATIC */
vkwp::CommandBuffer
vkwp::CommandBuffer::allocatePrimaryCommandBuffer(const std::string &          label,
                                                  const CommandPool &          command_pool,
                                                  const OptionalAllocateArgs & opt_args)
{
    return allocateCommandBuffer(label, command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, opt_args);
}

vkwp::CommandBuffer::CommandBuffer()
    : Resource{}
    , mutex_{ nullptr }
{}

vkwp::CommandBuffer::CommandBuffer(const std::string & label)
    : Resource{ label }
    , mutex_{ nullptr }
{}

vkwp::CommandBuffer::CommandBuffer(CommandBuffer && m) noexcept
    : Resource{ std::move(m) }
    , mutex_{ m.mutex_ }
{
    if (this != &m)
    {
        m.mutex_ = nullptr;
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
        rhs.mutex_                     = nullptr;
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
        .pNext            = nullptr,
        .flags            = flags,
        .pInheritanceInfo = nullptr,
    };
    VulkanError::check(  //
        vkBeginCommandBuffer(*this, &bi),
        "Failed to vkBeginCommandBuffer().");
}

void
vkwp::CommandBuffer::endCommandBuffer() const
{
    VulkanError::check(  //
        vkEndCommandBuffer(*this),
        "Failed to vkEndCommandBuffer().");
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
