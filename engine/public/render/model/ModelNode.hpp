#pragma once

#include <R3>
#include <vector>

namespace R3 {

struct R3_API ModelNode {
    usize skin;
    usize mesh;
    std::vector<float> weights;
    std::vector<ModelNode> children;
};

} // namespace R3