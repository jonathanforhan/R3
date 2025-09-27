#include "engine/render/Shader.hpp"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <istream>
#include <memory>
#include <span>
#include <utility>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <spirv.hpp>
#include <spirv_common.hpp>
#include <spirv_cross.hpp>
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "engine/api/MovableHandle.hpp"
#include "engine/render/Flags.hpp"
#include "render/ShaderObjects.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-ShaderMetadata.hpp"

extern VkDevice g_device;

namespace R3 {

Shader::Shader(const std::filesystem::path& filename, ShaderStageFlags stage)
    : m_stage{stage},
      m_metadata{std::make_unique<ShaderMetadata>()} {
    auto spirvCode = readFile(filename);
    createFromSource(spirvCode);
    populateShaderMetadata(spirvCode);
}

Shader::~Shader() noexcept {
    vkDestroyShaderModule(g_device, m_shader, nullptr);
}

const ShaderMetadata& Shader::metadata() const noexcept {
    return *m_metadata.get();
}

void Shader::createFromSource(std::span<const uint32> spirvCode) {
    const VkShaderModuleCreateInfo shaderModuleCreateInfo = {
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = spirvCode.size() * sizeof(uint32_t),
        .pCode    = spirvCode.data(),
    };
    VK_CHECK(vkCreateShaderModule(g_device, &shaderModuleCreateInfo, nullptr, &*m_shader));
}

void Shader::populateShaderMetadata(std::span<const uint32> spirvCode) {
    spirv_cross::Compiler compiler{spirvCode.data(), spirvCode.size()};
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    // push constants
    for (const auto& res : resources.push_constant_buffers) {
        uint32 size   = (uint32)compiler.get_declared_struct_size(compiler.get_type(res.base_type_id));
        uint32 offset = 0;

        m_metadata.get()->pushConstantRanges.push_back({
            .stageFlags = m_stage,
            .offset     = offset,
            .size       = size,
        });
    }

    // vertex input bindings
    // this has to conform to R3 Vertex struct
    m_metadata.get()->vertexInputBindingDescriptions.push_back({
        .binding   = 0,
        .stride    = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    });

    // vertex input attributes
    if (m_stage == ShaderStage::Vertex) {
        std::vector<VkVertexInputAttributeDescription> attrs;

        for (const auto& res : resources.stage_inputs) {
            uint32 location  = compiler.get_decoration(res.id, spv::DecorationLocation);
            const auto& type = compiler.get_type(res.type_id);

            VkVertexInputAttributeDescription attr = {
                .location = location,
                .binding  = 0,
                .format   = VK_FORMAT_UNDEFINED, // to be filled below
                .offset   = 0,
            };

            // clang-format off
            static constexpr VkFormat SFLOAT_FORMAT[4] = {VK_FORMAT_R32_SFLOAT, VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32B32A32_SFLOAT};
            static constexpr VkFormat SINT_FORMAT[4]   = {VK_FORMAT_R32_SINT,   VK_FORMAT_R32G32_SINT,   VK_FORMAT_R32G32B32_SINT,   VK_FORMAT_R32G32B32A32_SINT};
            static constexpr VkFormat UINT_FORMAT[4]   = {VK_FORMAT_R32_UINT,   VK_FORMAT_R32G32_UINT,   VK_FORMAT_R32G32B32_UINT,   VK_FORMAT_R32G32B32A32_UINT};
            // clang-format on

            // Map type to format
            switch (type.basetype) {
                case spirv_cross::SPIRType::Float:
                    attr.format = SFLOAT_FORMAT[type.vecsize - 1];
                    break;
                case spirv_cross::SPIRType::Int:
                    attr.format = SINT_FORMAT[type.vecsize - 1];
                    break;
                case spirv_cross::SPIRType::UInt:
                    attr.format = UINT_FORMAT[type.vecsize - 1];
                    break;
                default:
                    attr.format = VK_FORMAT_R32G32B32A32_SFLOAT;
                    break;
            }

            attrs.push_back(attr);
        }

        // Sort by location
        std::sort(attrs.begin(), attrs.end(), [](const auto& a, const auto& b) { return a.location < b.location; });

        // Calculate offsets

        for (uint32 offset = 0; auto& attr : attrs) {
            attr.offset = offset;

            uint32 size = 0;
            switch (attr.format) {
                case VK_FORMAT_R32_SFLOAT:
                case VK_FORMAT_R32_SINT:
                case VK_FORMAT_R32_UINT:
                    size = 4;
                    break;
                case VK_FORMAT_R32G32_SFLOAT:
                case VK_FORMAT_R32G32_SINT:
                case VK_FORMAT_R32G32_UINT:
                    size = 8;
                    break;
                case VK_FORMAT_R32G32B32_SFLOAT:
                case VK_FORMAT_R32G32B32_SINT:
                case VK_FORMAT_R32G32B32_UINT:
                    size = 12;
                    break;
                case VK_FORMAT_R32G32B32A32_SFLOAT:
                case VK_FORMAT_R32G32B32A32_SINT:
                case VK_FORMAT_R32G32B32A32_UINT:
                    size = 16;
                    break;
                default:
                    size = 16;
                    break;
            }

            offset += size;
        }

        m_metadata.get()->vertexInputAttributeDescriptions = std::move(attrs);
    }
}

std::vector<uint32_t> Shader::readFile(const std::filesystem::path& filename) const {
    std::ifstream file{filename, std::ios::ate | std::ios::binary};

    if (!(file.is_open() && file.good())) {
        throw Exception{std::format("Failed to open shader file: {}", filename.string())};
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    file.close();

    return buffer;
}

} // namespace R3
