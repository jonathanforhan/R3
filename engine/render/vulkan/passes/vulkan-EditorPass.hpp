#pragma once

#include "../vulkan-Fwd.hpp"
#include "../vulkan-RenderPass.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/Types.hpp"

namespace R3::vulkan {

class EditorPass : public RenderPass {
public:
    R3_CTOR_DEFAULT(EditorPass);
    R3_COPY_DELETE(EditorPass);
    R3_MOVE_DEFAULT(EditorPass);

    virtual ~EditorPass() noexcept = default;

protected:
    virtual void render(CommandBuffer& cmd) override;

protected:
    GraphicsPipeline* m_mainPipeline = nullptr;
};

} // namespace R3::vulkan
