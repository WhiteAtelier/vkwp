#include "vkwp/device_memory_node.hpp"

#include "impl/assert.hpp"
#include "impl/device_memory_block.hpp"
#include "impl/device_memory_block_node.hpp"
#include "vkwp/buffer.hpp"
#include "vkwp/image.hpp"
#include "vkwp/utils.hpp"
#include "vkwp/vulkan_error.hpp"

vkwp::DeviceMemoryNode::DeviceMemoryNode()
    : label_{}
    , mapping_alignment_{ 0 }
    , block_{ nullptr }
    , node_{ nullptr }
{}

vkwp::DeviceMemoryNode::DeviceMemoryNode(const std::string & label)
    : label_{ label }
    , mapping_alignment_{ 0 }
    , block_{ nullptr }
    , node_{ nullptr }
{}

vkwp::DeviceMemoryNode::DeviceMemoryNode(const std::string &           label,
                                         const std::size_t             mapping_alignment,
                                         DeviceMemoryBlock * const     block,
                                         DeviceMemoryBlockNode * const node)
    : label_{ label }
    , mapping_alignment_{ mapping_alignment }
    , block_{ block }
    , node_{ node }
{}

vkwp::DeviceMemoryNode::DeviceMemoryNode(DeviceMemoryNode && m) noexcept
    : label_{ std::move(m.label_) }
    , mapping_alignment_{ m.mapping_alignment_ }
    , block_{ m.block_ }
    , node_{ m.node_ }
{
    m.block_ = nullptr;
    m.node_  = nullptr;
}

vkwp::DeviceMemoryNode::~DeviceMemoryNode() noexcept
{
    this->reset();
}

vkwp::DeviceMemoryNode &
vkwp::DeviceMemoryNode::operator=(DeviceMemoryNode && m) noexcept
{
    if (this != &m)
    {
        this->label_             = std::move(m.label_);
        this->mapping_alignment_ = m.mapping_alignment_;
        this->block_             = m.block_;
        this->node_              = m.node_;
        m.mapping_alignment_     = 0;
        m.block_                 = nullptr;
        m.node_                  = nullptr;
    }
    return *this;
}

bool
vkwp::DeviceMemoryNode::operator!() const & noexcept
{
    return this->node_ == nullptr;
}

vkwp::DeviceMemoryNode::operator bool() const & noexcept
{
    return this->node_ != nullptr;
}

const std::string &
vkwp::DeviceMemoryNode::getLabel() const & noexcept
{
    return this->label_;
}

void
vkwp::DeviceMemoryNode::reset() & noexcept
{
    if (this->node_ != nullptr)
    {
        VKWP_ASSERTION(this->block_ != nullptr);
        this->block_->releaseNode(this->node_);
        this->node_  = nullptr;
        this->block_ = nullptr;
    }
}

std::size_t
vkwp::DeviceMemoryNode::getSize() const & noexcept
{
    if (this->node_ != nullptr)
    {
        return this->node_->getSize();
    }
    else
    {
        return 0;
    }
}

namespace {
struct Scope
{
private:
    VkDevice       device_;
    VkDeviceMemory memory_;
    void *         ptr_;

public:
    Scope(const VkDevice device, const VkDeviceMemory memory, const std::size_t size, const std::size_t offset)
        : device_{ device }
        , memory_{ memory }
    {
        vkwp::VulkanError::check(  //
            vkMapMemory(device, memory, offset, size, 0, &this->ptr_),
            "Failed to vkMapMemory().");
    }

    ~Scope() noexcept { vkUnmapMemory(this->device_, this->memory_); }

    void *
    operator()()
    {
        return this->ptr_;
    }
};
}  // namespace

std::size_t
vkwp::DeviceMemoryNode::_alignMappingRange(std::size_t & size, std::size_t & offset) const
{
    auto new_offset = utils::align(offset + 1, this->mapping_alignment_) - this->mapping_alignment_;
    VKWP_ASSERTION(new_offset <= offset);
    auto dst_offset = offset - new_offset;
    size            = utils::align(dst_offset + size, this->mapping_alignment_);
    offset          = new_offset;
    return dst_offset;
}

VkDeviceMemory
vkwp::DeviceMemoryNode::_getDeviceMemory() const
{
    return *this->block_;
}

std::size_t
vkwp::DeviceMemoryNode::_getOffset() const
{
    return this->node_->getOffset();
}

void
vkwp::DeviceMemoryNode::write(const void * const src, const std::size_t _size, const std::size_t _offset)
{
    // とにかくこの領域だけ map して書き込むモード.
    VKWP_ASSERTION(this->block_ != nullptr);
    VKWP_ASSERTION(*this->block_);

    // offset と size について, min mapping alignment に従って alignment を取る.
    auto       size       = _size;
    auto       offset     = _offset;
    const auto dst_offset = this->_alignMappingRange(size, offset);

    Scope scope{ this->block_->getDevice(), *this->block_, size, offset };
    std::memcpy(reinterpret_cast<std::uint8_t *>(scope()) + dst_offset, src, _size);
}

void
vkwp::DeviceMemoryNode::read(void * const dst, const std::size_t data_size, const std::size_t node_offset)
{
    // とにかくこの領域だけ map して読み込むモード.
    VKWP_ASSERTION(this->block_ != nullptr);
    VKWP_ASSERTION(*this->block_);

    // offset と size について, min mapping alignment に従って alignment を取る.
    auto       map_size   = data_size;
    auto       map_offset = node_offset;
    const auto dst_offset = this->_alignMappingRange(map_size, map_offset);

    Scope scope{ this->block_->getDevice(), *this->block_, map_size, map_offset };
    std::memcpy(dst, reinterpret_cast<std::uint8_t *>(scope()) + dst_offset, data_size);
}
