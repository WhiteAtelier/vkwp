#include "device_memory_block_node.hpp"

#include "assert.hpp"
#include "vkwp/utils.hpp"

vkwp::DeviceMemoryBlockNode::DeviceMemoryBlockNode(const std::size_t size) noexcept
    : prev_{ nullptr }
    , next_{}
    , size_{ size }
    , offset_{ 0 }
    , used_{ false }
{
    VKWP_ASSERTION(this->size_ > 0);
}

vkwp::DeviceMemoryBlockNode::~DeviceMemoryBlockNode() noexcept
{
    VKWP_ASSERTION(this->used_ == false);
}

vkwp::DeviceMemoryBlockNode *
vkwp::DeviceMemoryBlockNode::align(std::unique_ptr<DeviceMemoryBlockNode> & root_node, const std::size_t alignment)
{
    VKWP_ASSERTION(this->prev_ == nullptr || this->prev_->used_);
    VKWP_ASSERTION(this->used_);

    const auto new_offset = utils::align(this->offset_, alignment);
    if (this->offset_ != new_offset)
    {
        VKWP_ASSERTION(this->offset_ < new_offset);
        VKWP_ASSERTION((new_offset - this->offset_) < this->size_);

        // 縮小可能.
        auto new_prev_node     = std::make_unique<DeviceMemoryBlockNode>(new_offset - this->offset_);
        new_prev_node->offset_ = this->offset_;
        new_prev_node->prev_   = this->prev_;

        // 以降 noexcept
        this->offset_ = new_offset;           //  alignment 済みの offset
        this->size_ -= new_prev_node->size_;  // サイズ縮小

        if (this->prev_ != nullptr)
        {
            // 自分のノードを, 新しくできた前ノードの next に設定.
            new_prev_node->next_ = std::move(this->prev_->next_ /* == this */);

            // 自分のノードの prev を, 新しく作ったノードに差し替え
            this->prev_ = new_prev_node.get();

            // 新しくできた前ノードの, その前ノードの next に, 新しくできた前ノードを設定
            new_prev_node->prev_->next_ = std::move(new_prev_node);
        }
        else
        {
            // 自分が先頭ノード.
            // 自分のノードを, 新しくできた前ノードの next に設定.
            VKWP_ASSERTION(root_node.get() == this);
            new_prev_node->next_ = std::move(root_node /* == this */);

            // 自分のノードの prev を, 新しく作ったノードに差し替え
            this->prev_ = new_prev_node.get();

            // root_node に, 新しくできた前ノードを設定
            root_node = std::move(new_prev_node);
        }
    }
    return this->prev_;
}

vkwp::DeviceMemoryBlockNode *
vkwp::DeviceMemoryBlockNode::divide(const std::size_t new_size_of_this_node)
{
    VKWP_ASSERTION(this->used_);
    VKWP_ASSERTION(new_size_of_this_node <= this->size_);

    if (new_size_of_this_node < this->size_)
    {
        auto next_node = std::make_unique<DeviceMemoryBlockNode>(this->size_ - new_size_of_this_node);

        // 以降は noexcept.

        // next_node setup
        next_node->prev_   = this;
        next_node->offset_ = this->offset_ + new_size_of_this_node;
        next_node->next_   = std::move(this->next_);

        // this node setup
        this->next_ = std::move(next_node);
        this->size_ = new_size_of_this_node;
    }
    return this->next_.get();
}

std::unique_ptr<vkwp::DeviceMemoryBlockNode>
vkwp::DeviceMemoryBlockNode::tryToMergeNextNode()
{
    // 自分が free かつ next も free の場合のみ merge する
    if (!this->used_ && this->next_ && !this->next_->used_)
    {
        auto old_next = std::move(this->next_);

        // next / prev
        this->next_ = std::move(old_next->next_);
        if (this->next_)
        {
            this->next_->prev_ = this;
        }

        // size
        this->size_ += old_next->size_;

        // offset, used は変わらない.
        old_next->prev_ = nullptr;
        return old_next;
    }
    else
    {
        return nullptr;
    }
}

void
vkwp::DeviceMemoryBlockNode::setUsed(const bool used) noexcept
{
    VKWP_ASSERTION(this->used_ != used);
    this->used_ = used;
}

bool
vkwp::DeviceMemoryBlockNode::isUsed() const noexcept
{
    return this->used_;
}

std::size_t
vkwp::DeviceMemoryBlockNode::getSize() const noexcept
{
    return this->size_;
}

std::size_t
vkwp::DeviceMemoryBlockNode::getOffset() const noexcept
{
    return this->offset_;
}

vkwp::DeviceMemoryBlockNode *
vkwp::DeviceMemoryBlockNode::getPrev() const noexcept
{
    return this->prev_;
}

vkwp::DeviceMemoryBlockNode *
vkwp::DeviceMemoryBlockNode::getNext() const noexcept
{
    return this->next_.get();
}
