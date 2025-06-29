#ifndef VKWP_IMPL_ASSERT_HPP
#define VKWP_IMPL_ASSERT_HPP

#include <stdexcept>
#include <string>

/// @cond
namespace vkwp::impl {

///
/// @brief プログラムコーディングミスによるエラー.
///
/// このエラーは catch しないこと.
///
/// VKWP_CODING_ERROR(expr) の使用を推奨します.
///
class CodingError : public std::exception
{
public:
    // ========================================================================================= //
    /// @name Constructor / Destructor
    /// @{
    // ========================================================================================= //

    explicit CodingError(const std::string & expr,
                         const std::string & filename,
                         const int           line,
                         const std::string & funcname);

    ~CodingError() noexcept;

    /// @}
    // ========================================================================================= //
    /// @name Getter
    /// @{
    // ========================================================================================= //

    /// @brief エラーの内容を文字列として取得します.
    virtual const char *
    what() const noexcept override;

    /// @brief true になるべき検査条件を取得します.
    const std::string &
    getExpr() const;

    /// @brief エラー箇所のソースファイル名を取得します.
    const std::string &
    getFilename() const;

    /// @brief エラー箇所の行数を取得します.
    int
    getLine() const;

    /// @brief エラー箇所の関数名を取得します.
    const std::string &
    getFuncname() const;

    /// @}

private:
    const std::string expr_;
    const std::string filename_;
    const int         line_;
    const std::string funcname_;
    const std::string what_;
};

///
/// @brief assert の代わりに使用される Exception.
///
/// 通常デバッグ時では, アサーションには assert が使用されるが,
/// リリース版時にあえてアサーションを有効にしたい場合に, assert の代わりにこの例外が送出されます.
///
class AssertionError : public std::runtime_error
{
public:
    // ========================================================================================= //
    /// @name Constructor / Destructor
    /// @{
    // ========================================================================================= //

    explicit AssertionError(const std::string & expr,
                            const std::string & filename,
                            const int           line,
                            const std::string & funcname);

    ~AssertionError() noexcept;

    /// @}
    // ========================================================================================= //
    /// @name Getter
    /// @{
    // ========================================================================================= //

    /// @brief true になるべき検査条件を取得します.
    const std::string &
    getExpr() const;

    /// @brief エラー箇所のソースファイル名を取得します.
    const std::string &
    getFilename() const;

    /// @brief エラー箇所の行数を取得します.
    int
    getLine() const;

    /// @brief エラー箇所の関数名を取得します.
    const std::string &
    getFuncname() const;

    /// @}

    // spdlog を隠ぺいするため static 関数を設けた.
    static void
    printMessage(const std::string & expr, const std::string & file, const int line, const std::string & func);

    static void
    printMessage(const std::string & expr,
                 const std::string & file,
                 const int           line,
                 const std::string & func,
                 const std::string & message);

private:
    const std::string expr_;
    const std::string filename_;
    const int         line_;
    const std::string funcname_;
};

}  // namespace vkwp::impl
/// @endcond

#if VKWP_DEBUG
#    include <cassert>
#    define VKWP_ASSERTION(expr)                                                             \
        if (!(expr))                                                                         \
        {                                                                                    \
            ::vkwp::impl::AssertionError::printMessage(#expr, __FILE__, __LINE__, __func__); \
            assert(false && #expr);                                                          \
        }
#else
#    define VKWP_ASSERTION(expr)
#endif

#define VKWP_CODING_ERROR(expr)                                                          \
    if (!(expr))                                                                         \
    {                                                                                    \
        ::vkwp::impl::AssertionError::printMessage(#expr, __FILE__, __LINE__, __func__); \
        throw ::vkwp::impl::CodingError{ #expr, __FILE__, __LINE__, __func__ };          \
    }
#define VKWP_CODING_ERROR_M(expr, msg)                                                        \
    if (!(expr))                                                                              \
    {                                                                                         \
        ::vkwp::impl::AssertionError::printMessage(#expr, __FILE__, __LINE__, __func__, msg); \
        throw ::vkwp::impl::CodingError{ #expr, __FILE__, __LINE__, __func__ };               \
    }

#endif
