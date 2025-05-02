#ifndef VKWP_QUEUE_HPP
#define VKWP_QUEUE_HPP

#include "fwd.hpp"
#include "mutex.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

#include <memory>

namespace vkwp {

///
/// @brief VkQueue wrapper.
///
class Queue final : public Resource<VkQueue, void, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]]
    explicit Queue();

    /// @brief Constructor with label name.
    [[nodiscard]]
    explicit Queue(const std::string & label);

    std::uint32_t
    getQueueFamilyIndex() const noexcept;

    void
    setExternal(const VkQueue &, std::unique_ptr<Mutex> && mutex = nullptr) & noexcept;

    /// @brief Lock this queue for synchronized.
    ///
    /// Should not use this function directly, but use with std::lock_guard, std::unique_lock, or etc...
    void
    lock() const;

    /// @brief Try to lock this queue for synchronized.
    ///
    /// Should not use this function directly, but use with std::lock_guard, std::unique_lock, or etc...
    bool
    try_lock() const;

    /// @brief Unlock this queue for synchronized.
    void
    unlock() const;

private:
    using Resource<VkQueue, void, void>::setExternal;

    friend Device;
    std::uint32_t          queue_family_index_;
    std::unique_ptr<Mutex> mutex_;
};

}  // namespace vkwp

#endif
