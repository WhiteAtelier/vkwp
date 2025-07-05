#ifndef VKWP_IMPL_DEVICE_MEMORY_BLOCK_NODE_HPP
#define VKWP_IMPL_DEVICE_MEMORY_BLOCK_NODE_HPP

#include <memory>

/// @cond
namespace vkwp {

class DeviceMemoryBlockNode final
{
public:
    DeviceMemoryBlockNode(const std::size_t size) noexcept;  ///< Create Initial Node
    DeviceMemoryBlockNode(const DeviceMemoryBlockNode &) = delete;
    DeviceMemoryBlockNode(DeviceMemoryBlockNode &&)      = delete;
    ~DeviceMemoryBlockNode() noexcept;

    /// @brief このノードの offset について alignment を取る.
    ///
    /// 要件として, このノードは "used" であることと,
    /// 前のノードがあるならば, それは使用中でなければならない.
    /// free ノードが連続することがないからである.
    ///
    /// @param alignment
    ///     アライメントサイズ.
    ///
    /// @return
    ///     もしこのノードの前に新しいノードが出来た場合は, そのノードに対する ptr.
    ///     既に alignment が取れていた場合は, 現在の prev ノード.
    DeviceMemoryBlockNode *
    align(std::unique_ptr<DeviceMemoryBlockNode> & root_node, const std::size_t alignment);

    /// @brief このノードを分割する.
    ///
    /// 要件として, このノードは "used" であること.
    ///
    /// @param new_size_of_this_node
    ///     このノードの新しいサイズ.
    ///     このノードのサイズ以下を指定する必要がある.
    ///
    /// @returns
    ///     分割後に作成された次の free ノード.
    ///     指定されたノードサイズが現在のサイズと同じ場合は, このノードの次のノードを返す.
    ///     次のノードが存在しない場合は nullptr.
    DeviceMemoryBlockNode *
    divide(const std::size_t new_size_of_this_node);

    // マージされた場合は, マージした結果消滅予定の node を返す.
    // このノードは再利用出来ない (prev も next も null)
    std::unique_ptr<DeviceMemoryBlockNode>
    tryToMergeNextNode();

    void
    setUsed(const bool used) noexcept;

    bool
    isUsed() const noexcept;

    std::size_t
    getSize() const noexcept;

    std::size_t
    getOffset() const noexcept;

    DeviceMemoryBlockNode *
    getPrev() const noexcept;

    DeviceMemoryBlockNode *
    getNext() const noexcept;

private:
    DeviceMemoryBlockNode *                prev_;
    std::unique_ptr<DeviceMemoryBlockNode> next_;
    std::size_t                            size_;
    std::size_t                            offset_;
    bool                                   used_;
};

}  // namespace vkwp
/// @endcond

#endif