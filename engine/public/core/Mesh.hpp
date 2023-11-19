#pragma once
#include <span>
#include <vector>
#include "api/Types.hpp"
#include "core/Vertex.hpp"

namespace R3 {

class Mesh {
public:
    Mesh() = default;
    Mesh(std::span<Vertex> vertices, std::span<uint32> indices = {});
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&& src) noexcept {
        this->~Mesh();
        m_vao = src.m_vao;
        m_vbo = src.m_vbo;
        m_ebo = src.m_ebo;
        m_vertexCount = src.m_vertexCount;
        m_indexCount = src.m_indexCount;
        m_textures = src.m_textures;
        src.m_vao = src.m_vbo = src.m_ebo = 0;
    }
    void operator=(const Mesh&) = delete;
    Mesh& operator=(Mesh&& src) noexcept {
        this->~Mesh();
        m_vao = src.m_vao;
        m_vbo = src.m_vbo;
        m_ebo = src.m_ebo;
        m_vertexCount = src.m_vertexCount;
        m_indexCount = src.m_indexCount;
        m_textures = src.m_textures;
        src.m_vao = src.m_vbo = src.m_ebo = 0;
        return *this;
    }
    ~Mesh();

    void bind();

    auto indexCount() const -> uint32 const { return m_indexCount; }
    auto vertexCount() const -> uint32 const { return m_vertexCount; }
    auto textures() const -> const std::vector<usize>& { return m_textures; }
    void addTextureIndex(usize i) { m_textures.push_back(i); }

private:
    uint32 m_vao{0};
    uint32 m_vbo{0};
    uint32 m_ebo{0};
    uint32 m_vertexCount{0};
    uint32 m_indexCount{0};
    std::vector<usize> m_textures;
};

} // namespace R3
