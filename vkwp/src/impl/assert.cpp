// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#include "assert.hpp"

#include <spdlog/spdlog.h>

#include <sstream>

vkwp::impl::CodingError::CodingError(const std::string & expr,
                                     const std::string & filename,
                                     const int           line,
                                     const std::string & funcname)
    : expr_{ expr }
    , filename_{ filename }
    , line_{ line }
    , funcname_{ funcname }
    , what_{ [&]() {
        std::stringstream sstr;
        sstr << "Coding Error. " << expr << " - " << filename << "#" << line << " " << funcname;
        return sstr.str();
    }() }
{}

vkwp::impl::CodingError::~CodingError() noexcept = default;

const char *
vkwp::impl::CodingError::what() const noexcept
{
    return this->what_.c_str();
}

const std::string &
vkwp::impl::CodingError::getExpr() const
{
    return this->expr_;
}

const std::string &
vkwp::impl::CodingError::getFilename() const
{
    return this->filename_;
}

int
vkwp::impl::CodingError::getLine() const
{
    return this->line_;
}

const std::string &
vkwp::impl::CodingError::getFuncname() const
{
    return this->funcname_;
}

// ============================================================================================= //

vkwp::impl::AssertionError::AssertionError(const std::string & expr,
                                           const std::string & filename,
                                           const int           line,
                                           const std::string & funcname)
    : std::runtime_error{ [&]() {
        std::stringstream sstr;
        sstr << "AssertionError (Release). " << expr << " - " << filename << "#" << line << " " << funcname;
        return sstr.str();
    }() }
    , expr_{ expr }
    , filename_{ filename }
    , line_{ line }
    , funcname_{ funcname }
{}

vkwp::impl::AssertionError::~AssertionError() noexcept = default;

const std::string &
vkwp::impl::AssertionError::getExpr() const
{
    return this->expr_;
}

const std::string &
vkwp::impl::AssertionError::getFilename() const
{
    return this->filename_;
}

int
vkwp::impl::AssertionError::getLine() const
{
    return this->line_;
}

const std::string &
vkwp::impl::AssertionError::getFuncname() const
{
    return this->funcname_;
}

void
vkwp::impl::AssertionError::printMessage(const std::string & expr,
                                         const std::string & file,
                                         const int           line,
                                         const std::string & func)
{
    SPDLOG_CRITICAL("Assertion Fault. {} - {}#{} {}", expr, file, line, func);
}

void
vkwp::impl::AssertionError::printMessage(const std::string & expr,
                                         const std::string & file,
                                         const int           line,
                                         const std::string & func,
                                         const std::string & message)
{
    SPDLOG_CRITICAL("Assertion Fault. {} - {}#{} {}", expr, file, line, func);
    SPDLOG_CRITICAL("-- Message: {}", message);
}
