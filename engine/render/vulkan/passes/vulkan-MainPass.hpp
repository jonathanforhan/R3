#pragma once

#include <span>
#include "../vulkan-Fwd.hpp"
#include "../vulkan-RenderPass.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/Types.hpp"

namespace R3::vulkan {

class MainPass : public RenderPass {
public:
    R3_CTOR_DEFAULT(MainPass);
    R3_COPY_DELETE(MainPass);
    R3_MOVE_DEFAULT(MainPass);

    virtual ~MainPass() noexcept = default;

    void setCubemapPipeline(GraphicsPipeline* pipeline) noexcept { m_cubemapPipeline = pipeline; }

    void setCubemapTextureSlot(uint32 slot) noexcept { m_cubemapTextureSlot = slot; }

    void setLightCount(uint32 count) noexcept { m_lightCount = count; }

protected:
    virtual void render(CommandBuffer& cmd) override;

    void renderCubemap(CommandBuffer& cmd);

    void renderScene(CommandBuffer& cmd);

protected:
    GraphicsPipeline* m_cubemapPipeline = nullptr;
    uint32 m_cubemapTextureSlot         = 0xFFFFFFFF;
    uint32 m_lightCount                 = 0;
};

} // namespace R3::vulkan
