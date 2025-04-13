#include "vkwp/device_memory_pool.hpp"

#include "impl/device_memory_pool_unit.hpp"
#include "impl/assert.hpp"
#include "vkwp/device_memory_node.hpp"

#include <spdlog/spdlog.h>

#include <format>

vkwp::DeviceMemoryPool::DeviceMemoryPool()
    : label_{}
    , device_{ nullptr }
    , unit_size_{ 0 }
    , max_size_{ 0 }
    , current_size_{ 0 }
    , memory_type_index_{ ~0u }
    , memory_type_bits_{ 0 }
    , unit_index_{ 0 }
    , memories_{}
{}

vkwp::DeviceMemoryPool::DeviceMemoryPool(const std::string & label,
                                                const VkDevice      device,
                                                const std::size_t   unit_size,
                                                const std::size_t   max_size,
                                                const std::uint32_t memory_type_index,
                                                const std::uint32_t memory_type_bits)
    : label_{ label }
    , device_{ device }
    , unit_size_{ unit_size }
    , max_size_{ max_size }
    , current_size_{ 0 }
    , memory_type_index_{ memory_type_index }
    , memory_type_bits_{ memory_type_bits }
    , unit_index_{ 0 }
    , memories_{}
{
    VKWP_ASSERTION(this->device_ != nullptr);
    VKWP_ASSERTION(this->unit_size_ > 0);
}

vkwp::DeviceMemoryPool::DeviceMemoryPool(DeviceMemoryPool && m) noexcept = default;

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
    return this->unit_size_;
}

vkwp::DeviceMemoryNode
vkwp::DeviceMemoryPool::allocate(const std::string & label, const VkMemoryRequirements & memory_requirements)
{
    if (memory_requirements.alignment == 0)
    {
        SPDLOG_ERROR("Invalid memory requirements. Alignment size is zero.");
        return DeviceMemoryNode{ label };
    }

    // memoryTypeBits を調査
    if ((this->memory_type_bits_ & memory_requirements.memoryTypeBits) != memory_requirements.memoryTypeBits)
    {
        SPDLOG_ERROR("Cannot allocate memory node. Mismatched memory type bits. Request(0x{:x}), Capability(0x{:x})",
                     memory_requirements.memoryTypeBits,
                     this->memory_type_bits_);
        return DeviceMemoryNode{ label };
    }

    // ノードを確保する.
    for (auto iter = this->memories_.lower_bound(memory_requirements.size); iter != this->memories_.end(); ++iter)
    {
        if (auto node = iter->second->allocateNode(label, memory_requirements); node)
        {
            // 確保できた. allocatable node size で map を更新する.
            auto unit = std::move(iter->second);
            this->memories_.erase(iter);
            const auto new_alloc_size = unit->getMaxAllocatableNodeSize();
            this->memories_.emplace(new_alloc_size, std::move(unit));
            return node;
        }
    }

    // 確保できなかったので, 新たにメモリユニットを足す.
    // メモリユニットサイズを計算
    const auto allocation_size
        = ((((std::max)(memory_requirements.size, this->unit_size_) - 1) / memory_requirements.alignment) + 1)
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
    auto allocated_unit
        = impl::DeviceMemoryPoolUnit::allocate(std::format("{}:Unit[{}]", this->label_, this->unit_index_),
                                               this->device_,
                                               this->memory_type_index_,
                                               allocation_size);
    ++this->unit_index_;

    // iter に対して allocateNode する
    auto node = allocated_unit->allocateNode(label, memory_requirements);
    if (node)
    {
        // 確保できた
        const auto new_alloc_size = allocated_unit->getMaxAllocatableNodeSize();
        this->memories_.emplace(new_alloc_size, std::move(allocated_unit));
    }
    else
    {
        // 確保できないときは invalid node を返し, allocated_unit も解放する.
        SPDLOG_ERROR("Failed to allocate memory node.");
    }
    return node;
}
