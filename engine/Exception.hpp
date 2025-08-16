#pragma once

#include <source_location>
#include <stdexcept>

namespace R3 {

class Exception : public std::runtime_error {
public:
    explicit Exception(auto&& msg, std::source_location source_location = std::source_location::current()) noexcept
        : std::runtime_error{msg},
          _source_location(source_location) {}

    virtual ~Exception() noexcept override {}

    Exception(const Exception&)            = default;
    Exception& operator=(const Exception&) = default;

    Exception(Exception&&) noexcept            = default;
    Exception& operator=(Exception&&) noexcept = default;

    constexpr auto file() const noexcept { return _source_location.file_name(); }

    constexpr auto function() const noexcept { return _source_location.function_name(); }

    constexpr auto line() const noexcept { return _source_location.line(); }

private:
    const std::source_location _source_location;
};

} // namespace R3
