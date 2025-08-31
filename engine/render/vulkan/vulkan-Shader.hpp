#pragma once

#include <filesystem>
#include <span>
#include <vector>
#include <vulkan/vulkan.h>
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/Types.hpp"
#include "vulkan-Fwd.hpp"
#include "vulkan-Handle.hpp"

namespace R3::vulkan {

class R3_API Shader {
public:
    R3_CTOR_DEFAULT(Shader);
    R3_COPY_DELETE(Shader);
    R3_MOVE_DEFAULT(Shader);

    Shader(RenderContext& ctx, const std::filesystem::path& filename, VkShaderStageFlagBits stage);

    ~Shader() noexcept;

    VkShaderModule shader() const noexcept { return m_shaderModule; }

    VkShaderStageFlagBits stage() const noexcept { return m_stage; }

private:
    void createFromSource(RenderContext& ctx, std::span<const uint32> spirvCode);

    std::vector<uint32> readFile(const std::filesystem::path& filename) const;

private:
    Handle<VkDevice> m_device;
    Handle<VkShaderModule> m_shaderModule;
    VkShaderStageFlagBits m_stage = {};
};

} // namespace R3::vulkan