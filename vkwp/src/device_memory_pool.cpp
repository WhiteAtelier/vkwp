#include "vkwp/device_memory_pool.hpp"

#include "impl/assert.hpp"
#include "impl/device_memory_block.hpp"
#include "impl/device_memory_block_node.hpp"
#include "vkwp/device.hpp"
#include "vkwp/device_memory_node.hpp"
#include "vkwp/physical_device.hpp"

#include <spdlog/spdlog.h>

#include <format>

/* STATIC */
vkwp::DeviceMemoryPool
vkwp::DeviceMemoryPool::create(const std::string &        label,
                               const Device &             device,
                               const PhysicalDevice &     physical_device,
                               const std::size_t          block_size,
                               const std::size_t          max_size,
                               const void *               allocate_memory_p_next,
                               const DebugUtilsResource * debug_util)
{
    return DeviceMemoryPool{ label, device, physical_device, block_size, max_size, allocate_memory_p_next, debug_util };
}

vkwp::DeviceMemoryPool::DeviceMemoryPool()
    : label_{}
    , device_{ nullptr }
    , memory_properties_{}
    , block_size_{ 0 }
    , max_size_{ 0 }
    , current_size_{ 0 }
    , unit_index_{ 0 }
    , allocate_memory_p_next_{ nullptr }
    , debug_util_{ nullptr }
{}

vkwp::DeviceMemoryPool::DeviceMemoryPool(const std::string &        label,
                                         const Device &             device,
                                         const PhysicalDevice &     physical_device,
                                         const std::size_t          block_size,
                                         const std::size_t          max_size,
                                         const void *               allocate_memory_p_next,
                                         const DebugUtilsResource * debug_util)
    : label_{ label }
    , device_{ device }
    , memory_properties_{ physical_device.getMemoryProperties() }
    , block_size_{ block_size }
    , max_size_{ max_size }
    , current_size_{ 0 }
    , unit_index_{ 0 }
    , mutex_{ std::make_unique<std::mutex>() }
    , memory_blocks_(this->memory_properties_.memoryTypeCount)
    , allocate_memory_p_next_{ allocate_memory_p_next }
    , debug_util_{ debug_util }
{
    VKWP_ASSERTION(this->device_ != nullptr);
    VKWP_ASSERTION(this->block_size_ > 0);
}

vkwp::DeviceMemoryPool::DeviceMemoryPool(DeviceMemoryPool &&) noexcept = default;

vkwp::DeviceMemoryPool::~DeviceMemoryPool() noexcept = default;

vkwp::DeviceMemoryPool &
vkwp::DeviceMemoryPool::operator=(DeviceMemoryPool && m) noexcept
    = default;

bool
vkwp::DeviceMemoryPool::operator!() const & noexcept
{
    return this->device_ == nullptr;
}

vkwp::DeviceMemoryPool::operator bool() const & noexcept
{
    return this->device_ != nullptr;
}

const std::string &
vkwp::DeviceMemoryPool::getLabel() const noexcept
{
    return this->label_;
}

std::size_t
vkwp::DeviceMemoryPool::getUnitSize() const noexcept
{
    return this->block_size_;
}

vkwp::DeviceMemoryNode
vkwp::DeviceMemoryPool::allocate(const std::string &          label,
                                 const VkMemoryPropertyFlags  memory_property_flags,
                                 const VkMemoryRequirements & memory_requirements)
{
    if (!*this)
    {
        SPDLOG_ERROR("Invalid DeviceMemoryPool instance.");
        return DeviceMemoryNode{ label };
    }

    if (memory_requirements.alignment == 0)
    {
        SPDLOG_ERROR("Invalid memory requirements. Alignment size is zero.");
        return DeviceMemoryNode{ label };
    }

    // memory type index を取得
    const auto [ret, index] = PhysicalDevice::getMemoryTypeIndex(this->memory_properties_,
                                                                 memory_property_flags,
                                                                 memory_requirements.memoryTypeBits);
    if (!ret)
    {
        // ふさわしいメモリが存在しない
        SPDLOG_ERROR("Failed to allocate memory pool. Unsupported memory type.");
        return DeviceMemoryNode{ label };
    }

    VKWP_ASSERTION(index < this->memory_blocks_.size());
    auto & memory_blocks = this->memory_blocks_[index];

    // ノードを確保する.
    VKWP_ASSERTION(this->mutex_);
    std::lock_guard lock{ *this->mutex_ };
    for (auto iter = memory_blocks.lower_bound(memory_requirements.size); iter != memory_blocks.end(); ++iter)
    {
        if (auto node = iter->second->allocateNode(label, memory_requirements.size, memory_requirements.alignment);
            node)
        {
            // 確保できた. allocatable node size で map を更新する.
            auto unit = std::move(iter->second);
            memory_blocks.erase(iter);
            const auto new_alloc_size = unit->getMaxAllocatableNodeSize();
            memory_blocks.emplace(new_alloc_size, std::move(unit));
            return node;
        }
    }

    // 確保できなかったので, 新たにメモリユニットを足す.
    // メモリユニットサイズを計算
    const auto allocation_size
        = ((((std::max)(memory_requirements.size, this->block_size_) - 1) / memory_requirements.alignment) + 1)
        * memory_requirements.alignment;

    // 新規メモリユニットを作成
    if ((this->current_size_ + allocation_size) > this->max_size_)
    {
        // サイズ制限
        SPDLOG_ERROR(
            "Cannot allocate memory node. Size limitation. Required(={}Bytes) + Current(={}Bytes) > Max(={}Bytes).",
            allocation_size,
            this->current_size_,
            this->max_size_);
        return DeviceMemoryNode{ label };
    }

    // 新しいメモリを追加, まだ memories には足さない.
    auto allocated_unit = DeviceMemoryBlock::allocate(  //
        std::format("{}:Unit[{}]", this->label_, this->unit_index_),
        this->device_,
        index,
        allocation_size,
        (memory_property_flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0,
        this->allocate_memory_p_next_,
        this->debug_util_);
    ++this->unit_index_;

    // iter に対して allocateNode する
    auto node = allocated_unit->allocateNode(label, memory_requirements.size, memory_requirements.alignment);
    if (node)
    {
        // 確保できた
        const auto new_alloc_size = allocated_unit->getMaxAllocatableNodeSize();
        memory_blocks.emplace(new_alloc_size, std::move(allocated_unit));
    }
    else
    {
        // 確保できないときは invalid node を返し, allocated_unit も解放する.
        SPDLOG_ERROR("Failed to allocate memory node.");
    }
    return node;
}
