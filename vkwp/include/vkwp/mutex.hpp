#ifndef VKWP_MUTEX_HPP
#define VKWP_MUTEX_HPP

#include <memory>
#include <mutex>

namespace vkwp {

/// @brief Queue などで使用できる Mutex オブジェクトに対するコンテナのベースクラス.
class Mutex
{
public:
    Mutex() noexcept          = default;
    virtual ~Mutex() noexcept = default;
    Mutex(const Mutex &)      = delete;
    Mutex(Mutex &&)           = delete;

    /// @brief Lock mutex.
    virtual void
    lock()
        = 0;

    /// @brief Unlock mutex.
    virtual void
    unlock()
        = 0;

    /// @brief Try to lock mutex.
    ///
    /// @return If true, locked. Otherwise, failed to lock; mutex is locked by another thread.
    virtual bool
    tryLock()
        = 0;
};

/// @brief mutex オブジェクトを自前で用意するコンテナ.
///
/// mutex オブジェクトの共有は出来ない.
template <typename MutexT = std::mutex>
class StdMutex : public Mutex
{
public:
    using MutexType = MutexT;

    StdMutex();

    void
    lock() override;

    void
    unlock() override;

    bool
    tryLock() override;

protected:
    MutexT mutex_;
};

/// @brief mutex オブジェクトの参照を保持するコンテナ.
///
/// mutex オブジェクト本体の寿命に注意して利用する必要がある.
template <typename MutexT = std::mutex>
class StdRefMutex : public Mutex
{
public:
    using MutexType = MutexT;

    StdRefMutex(MutexT & ref);

    void
    lock() override;

    void
    unlock() override;

    bool
    tryLock() override;

protected:
    MutexT & mutex_;
};

/// @brief mutex オブジェクトを shared_ptr に包んで保持するコンテナ.
///
/// オーバーヘッドはあるものの, 寿命管理は気にしなくてよくなる.
/// またコンストラクタに nullptr は指定できない.
template <typename MutexT = std::mutex>
class StdSharedPtrMutex : public Mutex
{
public:
    using MutexType = MutexT;

    StdSharedPtrMutex(std::shared_ptr<MutexT> mutex);

    void
    lock() override;

    void
    unlock() override;

    bool
    tryLock() override;

protected:
    std::shared_ptr<MutexT> mutex_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief mutex object としてポインタを取る lock_guard.
///
/// コンストラクタに渡す mutex object は nullptr でも良い.
/// その場合, このクラスは何もしない.
///
/// @tparam MutexT
///     mutex object type.
///
template <class MutexT>
class LockGuardOpt
{
public:
    LockGuardOpt(MutexT * const mutex);
    ~LockGuardOpt();
    LockGuardOpt(const LockGuardOpt &) = delete;
    LockGuardOpt(LockGuardOpt &&)      = delete;

private:
    MutexT * const mutex_;
};

}  // namespace vkwp

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// StdMutex class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename MutexT>
inline vkwp::StdMutex<MutexT>::StdMutex() = default;

template <typename MutexT>
inline void
vkwp::StdMutex<MutexT>::lock()
{
    this->mutex_.lock();
}

template <typename MutexT>
inline void
vkwp::StdMutex<MutexT>::unlock()
{
    this->mutex_.unlock();
}

template <typename MutexT>
inline bool
vkwp::StdMutex<MutexT>::tryLock()
{
    return this->mutex_.try_lock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// StdRefMutex class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename MutexT>
inline vkwp::StdRefMutex<MutexT>::StdRefMutex(MutexT & ref)
    : mutex_{ ref }
{}

template <typename MutexT>
inline void
vkwp::StdRefMutex<MutexT>::lock()
{
    this->mutex_.lock();
}

template <typename MutexT>
inline void
vkwp::StdRefMutex<MutexT>::unlock()
{
    this->mutex_.unlock();
}

template <typename MutexT>
inline bool
vkwp::StdRefMutex<MutexT>::tryLock()
{
    return this->mutex_.try_lock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// StdSharedPtrMutex class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename MutexT>
inline vkwp::StdSharedPtrMutex<MutexT>::StdSharedPtrMutex(std::shared_ptr<MutexT> mutex)
    : mutex_{ std::move(mutex) }
{}

template <typename MutexT>
inline void
vkwp::StdSharedPtrMutex<MutexT>::lock()
{
    this->mutex_->lock();
}

template <typename MutexT>
inline void
vkwp::StdSharedPtrMutex<MutexT>::unlock()
{
    this->mutex_->unlock();
}

template <typename MutexT>
inline bool
vkwp::StdSharedPtrMutex<MutexT>::tryLock()
{
    return this->mutex_->try_lock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// StdSharedPtrMutex class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename MutexT>
vkwp::LockGuardOpt<MutexT>::LockGuardOpt(MutexT * const mutex)
    : mutex_{ mutex }
{
    if (this->mutex_ != nullptr)
    {
        this->mutex_->lock();
    }
}

template <typename MutexT>
vkwp::LockGuardOpt<MutexT>::~LockGuardOpt()
{
    if (this->mutex_ != nullptr)
    {
        this->mutex_->unlock();
    }
}

#endif
