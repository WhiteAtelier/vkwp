#if 0
#    define VKWP_VK_STRUCTURES_HPP

#    include "vulkan.hpp"

#    include <optional>

namespace vkwp {

class DeviceQueueCreateInfo
{
public:
    DeviceQueueCreateInfo() noexcept;
    DeviceQueueCreateInfo(DeviceQueueCreateInfo &&) = delete;
    DeviceQueueCreateInfo(VkDeviceQueueCreateInfo &&) noexcept;

    template <typename T>
    DeviceQueueCreateInfo &
    pNext(const T & vk_structure);

    DeviceQueueCreateInfo &
    flags(const VkDeviceQueueCreateFlags flags);

    DeviceQueueCreateInfo &
    queueFamilyIndex(const std::uint32_t count);

    DeviceQueueCreateInfo &
    queuePriorities(const float * const arr, const std::size_t size);

    operator VkDeviceQueueCreateInfo() const & = delete;
    operator VkDeviceQueueCreateInfo() &&;
    operator const VkDeviceQueueCreateInfo &() const &;

private:
    VkDeviceQueueCreateInfo data_;
};

}  // namespace vkwp

#endif