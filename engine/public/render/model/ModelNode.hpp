#pragma once

#include <R3>

namespace R3 {

struct R3_API ModelNode {
    uint32 skin;
    uint32 mesh;
    std::vector<float> weights;
    std::vector<uint32> children;
};

} // namespace R3