#pragma once

#include "Types.hpp"

namespace R3 {

consteval usize kilobyte(usize x) {
    return x * 1024;
}

consteval usize megabyte(usize x) {
    return x * 1024 * 1024;
}

consteval usize gigabyte(usize x) {
    return x * 1024 * 1024 * 1024;
}

} // namespace R3
