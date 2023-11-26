#pragma once
#include "render/NativeRenderObject.hpp"

namespace R3 {

struct GraphicsPipelineSpecification {
};

class GraphicsPipeline : public NativeRenderObject {
public:
    void create();
    void destroy();

private:
    GraphicsPipelineSpecification m_spec;
};

} // namespace R3
