#pragma once

#include <source_location>
#include <stdexcept>
#include "engine/api/Api.hpp"

namespace R3 {

class Exception : public std::runtime_error {
public:
    explicit Exception(auto&& msg, std::source_location source_location = std::source_location::current()) noexcept
        : std::runtime_error{msg},
          m_source_location(source_location) {}

    virtual ~Exception() noexcept override {}

    constexpr auto file() const noexcept { return m_source_location.file_name(); }

    constexpr auto function() const noexcept { return m_source_location.function_name(); }

    constexpr auto line() const noexcept { return m_source_location.line(); }

private:
    const std::source_location m_source_location;
};

} // namespace R3
