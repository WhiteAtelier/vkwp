#ifndef VKWP_DEVICE_MEMORY_HPP
#define VKWP_DEVICE_MEMORY_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

namespace vkwp {

///
/// @brief VkDeviceMemory wrapper.
///
class DeviceMemory final : public Resource<VkDeviceMemory, VkDevice>
{
public:
    using Resource::Resource;

    struct OptionalAllocateArgs
    {
        const void *               p_next      = nullptr;
        const DebugUtilsResource * debug_utils = nullptr;  ///< Pointer to a debug utils resource instance.
    };

    [[nodiscard]]
    static DeviceMemory
    allocate(const std::string &          label,
             const Device &               device,
             const PhysicalDevice &       physical_device,
             const VkMemoryRequirements & memory_requirements,
             const VkMemoryPropertyFlags  memory_property_flags,
             const OptionalAllocateArgs & opt_args = {});

    [[nodiscard]]
    static DeviceMemory
    importWin32ExternalMemory(const std::string &                      label,
                              const Device &                           device,
                              const PhysicalDevice &                   physical_device,
                              const std::size_t                        size,
                              const VkExternalMemoryHandleTypeFlagBits handle_type,
                              void * const                             handle,
                              const wchar_t * const                    name,
                              const VkImage &                          image,
                              const VkBuffer &                         buffer,
                              const OptionalAllocateArgs &             opt_args = {});

    /// @brief Write data.
    ///
    /// @param src
    ///     Source data.
    ///
    /// @param size
    ///     Source data size in bytes.
    ///
    /// @param offset
    ///     Destination buffer offset in bytes.
    void
    write(const void * const src, const std::size_t size, const std::size_t offset = 0) const;

    /// @brief Read data.
    ///
    /// @param dst
    ///     Destination buffer.
    ///
    /// @param size
    ///     Read size in bytes.
    ///
    /// @param offset
    ///     Source buffer offset in bytes.
    void
    read(void * const dst, const std::size_t size, const std::size_t offset = 0) const;

    class MappedScope final
    {
    private:
        friend DeviceMemory;
        MappedScope(const VkDevice &       device,
                    const VkDeviceMemory & device_memory,
                    const std::size_t      offset,
                    const std::size_t      size);

    public:
        MappedScope() noexcept;  ///< Default (invalid) scope constructor.
        MappedScope(const MappedScope &) = delete;
        MappedScope(MappedScope &&) noexcept;  ///< Move constructor.
        ~MappedScope() noexcept;               ///< Destructor. Unmapping automatically (if mapped).

        MappedScope &
        operator=(const MappedScope &)
            = delete;

        MappedScope &
        operator=(MappedScope &&) noexcept;  ///< Move assignable.

        /// @brief Is mapped?
        [[nodiscard]]
        explicit
        operator bool() const noexcept;

        /// @brief Is unmapped (invalid)?
        [[nodiscard]]
        bool
        operator!() const noexcept;

        /// @brief Is mapped?
        [[nodiscard]]
        bool
        isMapped() const noexcept;

        /// @brief Unmap if mapped.
        void
        unmap() noexcept;

        /// @brief Get mapped size in bytes.
        ///
        /// @returns
        ///     Mapped size in bytes. If unmapped, returns 0.
        [[nodiscard]]
        std::size_t
        size() const noexcept;

        /// @brief Get mapped memory pointer.
        ///
        /// @return
        ///     Pointer. If unmapped, returns nullptr.
        [[nodiscard]]
        void *
        ptr() noexcept;

        /// @brief Get mapped readonly memory pointer.
        ///
        /// @return
        ///     Pointer. If unmapped, returns nullptr.
        [[nodiscard]]
        const void *
        ptr() const noexcept;

    private:
        VkDevice       device_;
        VkDeviceMemory memory_;
        std::size_t    size_;
        void *         ptr_;
    };

    [[nodiscard]]
    MappedScope
    map(const std::size_t size, const std::size_t offset = 0) const;
};

}  // namespace vkwp

#endif
