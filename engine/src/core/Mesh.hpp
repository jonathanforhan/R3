#pragma once
#include <span>
#include "api/Types.hpp"
#include "core/Vertex.hpp"

namespace R3 {

class Mesh {
public:
    Mesh() = default;
    Mesh(std::span<Vertex> vertices, std::span<uint32> indices);
    void destroy();

    uint32 id() const { return _vao; };
    void bind() const;
    uint32 number_of_indices() const { return _number_of_indices; }

private:
    uint32 _vao{0};
    uint32 _vbo{0};
    uint32 _ebo{0};
    uint32 _number_of_indices{0};
};

} // namespace R3