#include "vkwp/queue.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"
#include "vkwp/mutex.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete<VkQueue>(VkQueue, const std::tuple<> &) noexcept
{
    // NOP
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Queue class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
vkwp::Queue::Queue()
    : Resource{}
    , queue_family_index_{ (std::numeric_limits<std::uint32_t>::max)() }
{}

vkwp::Queue::Queue(const std::string & label)
    : Resource{ label }
    , queue_family_index_{ (std::numeric_limits<std::uint32_t>::max)() }
{}

vkwp::Queue::Queue(const std::string &       label,
                   const VkQueue             external_queue,
                   const std::uint32_t       family_index,
                   std::unique_ptr<Mutex> && mutex)
    : Resource{ label, external_queue }
    , queue_family_index_{ family_index }
    , mutex_{ std::move(mutex) }
{}

std::uint32_t
vkwp::Queue::getQueueFamilyIndex() const noexcept
{
    // noexcept (no validate)
    return this->queue_family_index_;
}

void
vkwp::Queue::lock() const
{
    VKWP_VALIDATE_THIS
    if (this->mutex_)
    {
        this->mutex_->lock();
    }
}

bool
vkwp::Queue::try_lock() const
{
    VKWP_VALIDATE_THIS
    if (this->mutex_)
    {
        return this->mutex_->tryLock();
    }
    else
    {
        return true;
    }
}

void
vkwp::Queue::unlock() const
{
    VKWP_VALIDATE_THIS
    if (this->mutex_)
    {
        this->mutex_->unlock();
    }
}
