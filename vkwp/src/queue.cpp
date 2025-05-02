#include "vkwp/queue.hpp"

#include "impl/assert.hpp"
#include "vkwp/mutex.hpp"

vkwp::Queue::Queue() = default;

vkwp::Queue::Queue(const std::string & label)
    : Resource{ label }
    , queue_family_index_{ 0 }
    , mutex_{}
{}

std::uint32_t
vkwp::Queue::getQueueFamilyIndex() const noexcept
{
    return this->queue_family_index_;
}

void
vkwp::Queue::setExternal(const VkQueue & queue, std::unique_ptr<Mutex> && mutex) & noexcept
{
    static_cast<vkwp::Resource<VkQueue, void, void> *>(this)->setExternal(queue);
    this->mutex_ = std::move(mutex);
}

void
vkwp::Queue::lock() const
{
    if (this->mutex_)
    {
        this->mutex_->lock();
    }
}

bool
vkwp::Queue::try_lock() const
{
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
    if (this->mutex_)
    {
        this->mutex_->unlock();
    }
}
