#ifndef VKWP_VKM_RESOURCE_HPP
#define VKWP_VKM_RESOURCE_HPP

#include <string>
#include <type_traits>

namespace vkwp {

static_assert(std::is_copy_assignable_v<std::string>);
static_assert(std::is_nothrow_move_constructible_v<std::string>, "std::string must be nothrow move constructible.");
static_assert(std::is_nothrow_move_assignable_v<std::string>, "std::string must be nothrow move assignable.");

template <typename T>
concept vulkan_raw_resource = std::is_pointer_v<T>;
template <typename T>
concept vulkan_raw_resource_or_void = (std::is_pointer_v<T> || std::is_void_v<T>);

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief Vulkan Resource Base class.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
template <::vkwp::vulkan_raw_resource T>
class ResourceBase
{
public:
    using VkType = T;  ///< Vulkan Raw Resource Type.

    /// @brief Default Constructor.
    [[nodiscard]]
    explicit ResourceBase();

protected:
    /// @brief Constructor with label name.
    [[nodiscard]]
    explicit ResourceBase(const std::string & label);

    ResourceBase<T> &
    operator=(ResourceBase &&) & noexcept;  ///< Movable assignable to lvalue.

public:
    ResourceBase(const ResourceBase &) = delete;  ///< Uncopyable.
    ResourceBase(ResourceBase &&) noexcept;       ///< Movable.
    virtual ~ResourceBase() noexcept;

    ResourceBase<T> &
    operator=(const ResourceBase &)
        = delete;  ///< Copy unassignable.

    /// @brief 有効な Vulkan Resource を持っていないかどうか取得する.
    [[nodiscard]]
    bool
    operator!() const & noexcept;

    /// @brief 有効な Vulkan Resource を持っているかどうか取得する.
    [[nodiscard]]
    explicit
    operator bool() const & noexcept;

    /// @brief Vulkan Resource を取得する.
    [[nodiscard]] operator T() const & noexcept;

    [[nodiscard]]
    const T &
    operator*() const & noexcept;

    /// @brief Vulkan Resource ptr を取得する.
    [[nodiscard]]
    const T *
    ptr() const & noexcept;

    /// @brief この Vulkan Resource の名前を取得する.
    [[nodiscard]]
    const std::string &
    getLabel() const & noexcept;

    bool
    isExternal() const & noexcept;

    // clang-format off
    ResourceBase<T> & operator=(ResourceBase &&) && noexcept = delete; ///< ref-qualifier: rvalue deleted.
    bool operator!()  && noexcept                            = delete; ///< ref-qualifier: rvalue deleted.
    explicit operator bool() && noexcept                     = delete; ///< ref-qualifier: rvalue deleted.
    operator T() && noexcept                                 = delete; ///< ref-qualifier: rvalue deleted.
    const T * ptr() && noexcept                              = delete; ///< ref-qualifier: rvalue deleted.
    const std::string & getLabel() && noexcept               = delete; ///< ref-qualifier: rvalue deleted.
    // clang-format on

protected:
    /// @brief インスタンス変数本体への const 参照を取得する.
    [[nodiscard]]
    const T &
    _ref() const noexcept;

    /// @brief インスタンス変数本体への参照を取得する.
    [[nodiscard]]
    T &
    _ref() noexcept;

private:
    std::string label_;
    T           instance_;

protected:
    bool external_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief Vulkan Resource with Parent and Pool class.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
template <::vkwp::vulkan_raw_resource         T,
          ::vkwp::vulkan_raw_resource_or_void ParentT,
          ::vkwp::vulkan_raw_resource_or_void PoolT>
class Resource : public ResourceBase<T>
{
public:
    using VkParentType = ParentT;  ///< Parent Type.
    using VkPoolType   = PoolT;    ///< Pool Type.

protected:
    /// @brief Default Constructor.
    [[nodiscard]]
    explicit Resource();

    /// @brief Constructor with label name.
    [[nodiscard]]
    explicit Resource(const std::string & label);

public:
    Resource(Resource &&) noexcept;  ///< Movable.
    virtual ~Resource() noexcept;    ///< Destructible.

    Resource<T, ParentT, PoolT> &
    operator=(Resource &&) & noexcept;  ///< Move Assignable to lvalue.

    /// @brief 保持しているリソースを解放します.
    ///
    /// 有効なリソースを保持していない場合は何もしません.
    void
    reset() & noexcept;

    ///
    /// @brief 保持しているリソースを解放します.
    ///
    /// 有効なリソースを保持していない場合は何もしません.
    ///
    /// 解放済みの T 型変数へのポインタを返します.
    /// 新たにリソースを確保し, 保持する場合に使用します.
    ///
    /// @param parent
    ///     新しいリソースの parent resource.
    ///
    /// @param pool
    ///     新しいリソースの pool resource.
    ///
    /// @return
    ///     内部 T 型変数へのポインタ.
    ///     このポインタが指すリソースは解放済みです.
    ///
    T *
    reset(const ParentT & parent, const PoolT & pool) & noexcept;

    /// @brief 外部リソースとして vulkan resource を保持します.
    ///
    /// 外部管理のリソースを, このクラスで参照保持します.
    /// reset() 呼び出しの時に解放処理を行いません.
    ///
    /// また, この関数呼び出し時に有効なリソースを保持している場合, そのリソースに対しては解放処理を実行します.
    void
    setExternal(const T &, const ParentT & parent) & noexcept;

    ///
    /// @brief 保持しているリソースを解放せずに移譲します.
    ///
    /// 解放処理を行わず, リソースの所有権を外部へ移譲します.
    /// 呼び出したユーザは, このリソースについて責任を持って適宜解放する必要があります.
    ///
    /// @param parent
    ///     このリソースの parent resource の出力先変数.
    ///
    /// @param pool
    ///     このリソースの pool resource の出力先変数.
    ///
    [[nodiscard("Vulkan Resource Leak")]]
    T
    release(ParentT & parent, PoolT & pool, bool * const external = nullptr) noexcept;

    // clang-format off
    Resource<T, ParentT, PoolT> & operator=(Resource &&) && noexcept = delete; ///< ref-qualifier: rvalue deleted.
    void reset() && noexcept                                         = delete; ///< ref-qualifier: rvalue deleted.
    T * reset(const ParentT &, const PoolT &) && noexcept            = delete; ///< ref-qualifier: rvalue deleted.
    // clang-format on

protected:
    /// @brief Get Pool Instance.
    ///
    /// 何らかのリソースを保持している時に nullptr である場合, そのリソースは外部リソースである.
    const PoolT &
    _getPool() const noexcept;

    /// @brief Get Parent Instance.
    ///
    /// 何らかのリソースを保持している時に nullptr である場合, そのリソースは外部リソースである.
    const ParentT &
    _getParent() const noexcept;

private:
    friend class DebugUtilsResource;

    void
    _reset() noexcept;

    ParentT parent_;
    PoolT   pool_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief Vulkan Resource with Parent class.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
template <::vkwp::vulkan_raw_resource         T,  //
          ::vkwp::vulkan_raw_resource_or_void ParentT>
class Resource<T, ParentT, void> : public ResourceBase<T>
{
public:
    using VkParentType = ParentT;  ///< Parent Type.
    using VkPoolType   = void;     ///< No Pool.

protected:
    /// @brief Default Constructor.
    [[nodiscard]]
    explicit Resource();

    /// @brief Constructor with label name.
    [[nodiscard]]
    explicit Resource(const std::string & label);

public:
    Resource(Resource &&) noexcept;  ///< Move Constructable.
    virtual ~Resource() noexcept;    /// Destructible.

    Resource<T, ParentT, void> &
    operator=(Resource &&) & noexcept;  ///< Move Assignable to lvalue.

    /// @brief 保持しているリソースを解放します.
    ///
    /// 有効なリソースを保持していない場合は何もしません.
    void
    reset() & noexcept;

    ///
    /// @brief 保持しているリソースを解放します.
    ///
    /// 有効なリソースを保持していない場合は何もしません.
    ///
    /// 解放済みの T 型変数へのポインタを返します.
    /// 新たにリソースを確保し, 保持する場合に使用します.
    ///
    /// @param parent
    ///     新しいリソースの parent resource.
    ///
    /// @return
    ///     内部 T 型変数へのポインタ.
    ///     このポインタが指すリソースは解放済みです.
    ///
    T *
    reset(const ParentT & parent) & noexcept;

    /// @brief 外部リソースとして vulkan resource を保持します.
    ///
    /// 外部管理のリソースを, このクラスで参照保持します.
    /// reset() 呼び出しの時に解放処理を行いません.
    ///
    /// また, この関数呼び出し時に有効なリソースを保持している場合, そのリソースに対しては解放処理を実行します.
    void
    setExternal(const T &, const ParentT & parent) & noexcept;

    ///
    /// @brief 保持しているリソースを解放せずに移譲します.
    ///
    /// 解放処理を行わず, リソースの所有権を外部へ移譲します.
    /// 呼び出したユーザは, このリソースについて責任を持って適宜解放する必要があります.
    ///
    /// @param parent
    ///     このリソースの parent resource の出力先変数.
    ///
    [[nodiscard("Vulkan Resource Leak")]]
    T
    release(ParentT & parent, bool * const external = nullptr) noexcept;

    // clang-format off
    Resource<T, ParentT, void> & operator=(Resource &&) && noexcept = delete; ///< ref-qualifier: rvalue deleted.
    void reset() && noexcept                                        = delete; ///< ref-qualifier: rvalue deleted.
    T * reset(const ParentT &) && noexcept                          = delete; ///< ref-qualifier: rvalue deleted.
    void setExternal(const T &) && noexcept                         = delete; ///< ref-qualifier: rvalue deleted.
    // clang-format on

protected:
    /// @brief Get Parent Instance.
    ///
    /// 何らかのリソースを保持している時に nullptr である場合, そのリソースは外部リソースである.
    const ParentT &
    _getParent() const noexcept;

private:
    friend class DebugUtilsResource;

    void
    _reset() noexcept;

    ParentT parent_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief Vulkan Resource class (without Parent nor Pool).
///
///////////////////////////////////////////////////////////////////////////////////////////////////
template <::vkwp::vulkan_raw_resource T>
class Resource<T, void, void> : public ResourceBase<T>
{
protected:
    [[nodiscard]]
    explicit Resource();
    [[nodiscard]]
    explicit Resource(const std::string & label);

public:
    using VkParentType = void;  ///< No Parent.
    using VkPoolType   = void;  ///< No pool.

    Resource(Resource &&) noexcept;  ///< Move Constructable.
    virtual ~Resource() noexcept;    /// Destructible.

    Resource<T, void, void> &
    operator=(Resource &&) & noexcept;  ///< Move Assignable to lvalue.

    ///
    /// @brief 保持しているリソースを解放します.
    ///
    /// 有効なリソースを保持していない場合は何もしません.
    ///
    /// 解放済みの T 型変数へのポインタを返します.
    /// 新たにリソースを確保し, 保持する場合に使用します.
    ///
    /// @return
    ///     内部 T 型変数へのポインタ.
    ///     このポインタが指すリソースは解放済みです.
    ///
    T *
    reset() & noexcept;

    /// @brief 外部リソースとして vulkan resource を保持します.
    ///
    /// 外部管理のリソースを, このクラスで参照保持します.
    /// reset() 呼び出しの時に解放処理を行いません.
    ///
    /// また, この関数呼び出し時に有効なリソースを保持している場合, そのリソースに対しては解放処理を実行します.
    void
    setExternal(const T &) & noexcept;

    ///
    /// @brief 保持しているリソースを解放せずに移譲します.
    ///
    /// 解放処理を行わず, リソースの所有権を外部へ移譲します.
    /// 呼び出したユーザは, このリソースについて責任を持って適宜解放する必要があります.
    ///
    [[nodiscard("Vulkan Resource Leak")]]
    T
    release(bool * const external = nullptr) noexcept;

    // clang-format off
    Resource<T, void, void> & operator=(Resource &&) && noexcept = delete; ///< ref-qualifier: rvalue deleted.
    T * reset() && noexcept                                      = delete; ///< ref-qualifier: rvalue deleted.
    void setExternal(const T &) && noexcept                      = delete; ///< ref-qualifier: rvalue deleted.
    // clang-format on

private:
    void
    _reset() noexcept;
};

}  // namespace vkwp

#endif
