#include "vkwp/semaphore.hpp"

vkwp::Semaphore::Semaphore() = default;

vkwp::Semaphore::Semaphore(const std::string & label)
    : Resource{ label }
{}
