#pragma once
#include <set>
#include <string_view>
#include <vector>
#include "api/Types.hpp"
#include "core/Mesh.hpp"
#include "core/Shader.hpp"
#include "core/Texture2D.hpp"

namespace R3 {

class ModelComponent {
public:
    ModelComponent(const std::string& path, Shader& shader);

    auto mesh() -> Mesh& { return m_mesh; }
    auto shader() -> Shader& { return m_shader; };
    auto textures() -> std::vector<Texture2D>& { return m_textures; }
    auto path() const -> const std::string& { return m_directory + m_file; }

private:
    void checkVersion(std::string_view version) const;
    void checkVersion(uint32 major, uint32 minor) const;

public:
    mat4 transform{1.0f};
    vec2 tiling{1.0f};

private:
    Mesh m_mesh;
    Shader& m_shader;
    std::vector<Vertex> m_vertices;
    std::vector<uint32> m_indices;
    std::vector<Texture2D> m_textures;

    std::set<std::string> m_loadedTextures;
    std::string m_directory;
    std::string m_file;
};

} // namespace R3