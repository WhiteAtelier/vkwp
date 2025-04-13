#include "vkwp/queue.hpp"

#include "impl/assert.hpp"

vkwp::Queue::Queue() = default;

vkwp::Queue::Queue(const std::string & label)
    : Resource{ label }
{}
