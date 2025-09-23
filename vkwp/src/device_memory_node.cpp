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
    , block_{ nullptr }
    , node_{ nullptr }
{}

vkwp::DeviceMemoryNode::DeviceMemoryNode(const std::string & label)
    : label_{ label }
    , block_{ nullptr }
    , node_{ nullptr }
{}

vkwp::DeviceMemoryNode::DeviceMemoryNode(const std::string &           label,
                                         DeviceMemoryBlock * const     block,
                                         DeviceMemoryBlockNode * const node)
    : label_{ label }
    , block_{ block }
    , node_{ node }
{}

vkwp::DeviceMemoryNode::DeviceMemoryNode(DeviceMemoryNode && m) noexcept
    : label_{ std::move(m.label_) }
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
        this->label_ = std::move(m.label_);
        this->block_ = m.block_;
        this->node_  = m.node_;
        m.block_     = nullptr;
        m.node_      = nullptr;
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
vkwp::DeviceMemoryNode::write(const void * const src, const std::size_t size, const std::size_t _offset)
{
    // とにかくこの領域だけ map して書き込むモード.
    VKWP_ASSERTION(this->block_ != nullptr);
    VKWP_ASSERTION(*this->block_);

    const auto offset = _offset + this->_getOffset();  // MemoryBlock 先頭からの offset

    // 範囲外チェック
    if ((offset + size) > (this->_getOffset() + this->getSize()))
    {
        throw std::out_of_range{ "Out of range, Failed to write to DeviceMemoryNode." };
    }

    void * ptr = this->block_->getMappedPtr();
    if (ptr == nullptr)
    {
        throw std::runtime_error{ "Device memory cannot map. (not HOST_VISIBLE)" };
    }

    std::memcpy(reinterpret_cast<std::uint8_t *>(ptr) + offset, src, size);
}

void
vkwp::DeviceMemoryNode::read(void * const dst, const std::size_t size, const std::size_t _offset)
{
    // とにかくこの領域だけ map して読み込むモード.
    VKWP_ASSERTION(this->block_ != nullptr);
    VKWP_ASSERTION(*this->block_);

    const auto offset = _offset + this->_getOffset();  // MemoryBlock 先頭からの offset

    // 範囲外チェック
    if ((offset + size) > (this->_getOffset() + this->getSize()))
    {
        throw std::out_of_range{ "Out of range, Failed read to DeviceMemoryNode." };
    }

    void * ptr = this->block_->getMappedPtr();
    if (ptr == nullptr)
    {
        throw std::runtime_error{ "Device memory cannot map. (not HOST_VISIBLE)" };
    }

    std::memcpy(dst, reinterpret_cast<std::uint8_t *>(ptr) + offset, size);
}
