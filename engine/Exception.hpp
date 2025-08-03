#pragma once

#include <source_location>
#include <stdexcept>

namespace R3 {

class Exception : public std::runtime_error {
public:
    explicit Exception(auto&& msg, std::source_location source_location = std::source_location::current())
        : std::runtime_error{msg},
          _source_location(source_location) {}

    virtual ~Exception() noexcept {}

    constexpr auto file() const { return _source_location.file_name(); }

    constexpr auto function() const { return _source_location.function_name(); }

    constexpr auto line() const { return _source_location.line(); }

private:
    const std::source_location _source_location;
};

} // namespace R3
