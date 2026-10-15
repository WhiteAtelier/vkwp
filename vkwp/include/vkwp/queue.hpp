// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#ifndef VKWP_QUEUE_HPP
#define VKWP_QUEUE_HPP

#include "fwd.hpp"

#include "mutex.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

#include <cstdint>
#include <memory>
#include <string>

namespace vkwp {

///
/// @brief VkQueue wrapper.
///
class Queue final : public Resource<VkQueue, std::nullptr_t>
{
public:
    explicit Queue();

    [[nodiscard]]
    explicit Queue(std::string &&            label,
                   const VkQueue             external_queue,
                   const std::uint32_t       family_index,
                   std::unique_ptr<Mutex> && mutex);

    [[nodiscard]]
    std::uint32_t
    getQueueFamilyIndex() const noexcept;

    /// @brief Lock this queue for synchronized.
    ///
    /// Should not use this function directly, but use with std::lock_guard, std::unique_lock, or etc...
    void
    lock() const;

    /// @brief Try to lock this queue for synchronized.
    ///
    /// Should not use this function directly, but use with std::lock_guard, std::unique_lock, or etc...
    [[nodiscard]]
    bool
    try_lock() const;

    /// @brief Unlock this queue for synchronized.
    void
    unlock() const;

private:
    explicit Queue(std::string && label);

    friend Device;
    std::uint32_t          queue_family_index_;
    std::unique_ptr<Mutex> mutex_;
};

}  // namespace vkwp

#endif
