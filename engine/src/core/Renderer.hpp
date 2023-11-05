#pragma once
#include "api/Types.hpp"

namespace R3 {

enum class RenderPrimitive {
    Triangles,
    Lines,
    Points,
};

class Renderer {
private:
    Renderer();

public:
    ~Renderer();

    void predraw() const;
    void drawElements(RenderPrimitive primitive, uint32 indiceCount) const;
    void drawArrays(RenderPrimitive primitive, uint32 vertexCount) const;

private:
    friend class Engine;
};

} // namespace R3