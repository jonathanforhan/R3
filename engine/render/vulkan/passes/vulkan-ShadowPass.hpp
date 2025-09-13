#pragma once

#include "../vulkan-Fwd.hpp"
#include "../vulkan-RenderPass.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/Types.hpp"

namespace R3::vulkan {

class ShadowPass : public RenderPass {
public:
    R3_CTOR_DEFAULT(ShadowPass);
    R3_COPY_DELETE(ShadowPass);
    R3_MOVE_DEFAULT(ShadowPass);

    virtual ~ShadowPass() noexcept = default;

    void setLightSpaceMatrix(const fmat4& lightSpaceMatrix) noexcept { m_lightSpaceMatrix = lightSpaceMatrix; }

protected:
    virtual void setDynamicPipelineStates(CommandBuffer& cmd) override;

    virtual void render(CommandBuffer& cmd) override;

protected:
    fmat4 m_lightSpaceMatrix = fmat4(1.0);
};

} // namespace R3::vulkan
