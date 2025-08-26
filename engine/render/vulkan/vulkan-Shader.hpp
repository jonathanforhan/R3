#pragma once

#include <filesystem>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "api/Class.hpp"
#include "api/Types.hpp"
#include "vulkan-Fwd.hpp"
#include "vulkan-Handle.hpp"

namespace R3::vulkan {

class Shader {
public:
    R3_CTOR_DEFAULT(Shader);
    R3_COPY_DELETE(Shader);
    R3_MOVE_DEFAULT(Shader);

    Shader(RenderContext& ctx, const std::filesystem::path& filename, VkShaderStageFlags type);

    ~Shader() noexcept;

    VkShaderModule shader() const noexcept { return m_shaderModule; }

    VkShaderStageFlags type() const noexcept { return m_type; }

private:
    void createFromSource(RenderContext& ctx, std::span<const uint32> spirvCode, VkShaderStageFlags type);

    std::vector<uint32> readFile(const std::filesystem::path& filename) const;

private:
    Handle<VkDevice> m_device;
    Handle<VkShaderModule> m_shaderModule;
    VkShaderStageFlags m_type = {};
};

} // namespace R3::vulkan