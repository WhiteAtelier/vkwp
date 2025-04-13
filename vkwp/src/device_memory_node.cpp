#include "vkwp/device_memory_node.hpp"

#include "impl/device_memory_pool_unit.hpp"
#include "impl/assert.hpp"

vkwp::DeviceMemoryNode::DeviceMemoryNode()
    : label_{}
    , unit_{ nullptr }
    , node_ptr_{ nullptr }
    , memory_requirements_{}
    , node_offset_{ 0 }
{}

vkwp::DeviceMemoryNode::DeviceMemoryNode(const std::string & label)
    : label_{ label }
    , unit_{ nullptr }
    , node_ptr_{ nullptr }
    , memory_requirements_{}
    , node_offset_{ 0 }
{}

vkwp::DeviceMemoryNode::DeviceMemoryNode(const std::string &                label,
                                                impl::DeviceMemoryPoolUnit * const unit,
                                                void *                             node_ptr,
                                                const VkMemoryRequirements &       memory_requirements,
                                                const std::size_t                  node_offset)
    : label_{ label }
    , unit_{ unit }
    , node_ptr_{ node_ptr }
    , memory_requirements_{ memory_requirements }
    , node_offset_{ node_offset }
{}

vkwp::DeviceMemoryNode::DeviceMemoryNode(DeviceMemoryNode && m) noexcept
    : label_{ std::move(m.label_) }
    , unit_{ m.unit_ }
    , node_ptr_{ m.node_ptr_ }
    , memory_requirements_{ std::move(m.memory_requirements_) }
    , node_offset_{ m.node_offset_ }
{
    m.unit_     = nullptr;
    m.node_ptr_ = nullptr;
}

vkwp::DeviceMemoryNode::~DeviceMemoryNode() noexcept
{
    this->reset();
}

bool
vkwp::DeviceMemoryNode::operator!() const & noexcept
{
    return this->node_ptr_ == nullptr;
}

vkwp::DeviceMemoryNode::operator bool() const & noexcept
{
    return this->node_ptr_ != nullptr;
}

const std::string &
vkwp::DeviceMemoryNode::getLabel() const & noexcept
{
    return this->label_;
}

const VkMemoryRequirements &
vkwp::DeviceMemoryNode::getMemoryRequirements() const & noexcept
{
    return this->memory_requirements_;
}

void
vkwp::DeviceMemoryNode::reset() & noexcept
{
    if (this->node_ptr_ != nullptr)
    {
        VKWP_ASSERTION(this->unit_ != nullptr);
        this->unit_->releaseNode(this->node_ptr_);
        this->node_ptr_ = nullptr;
        this->unit_     = nullptr;
    }
}

std::size_t
vkwp::DeviceMemoryNode::getNodeOffset() const & noexcept
{
    return this->node_offset_;
}

void
vkwp::DeviceMemoryNode::write(const void * const src, const std::size_t size, const std::size_t offset)
{}

void
vkwp::DeviceMemoryNode::read(void * const dst, const std::size_t size, const std::size_t offset)
{}
