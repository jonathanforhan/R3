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

    void drawElements(RenderPrimitive primitive, uint32 indiceCount);

private:
    friend class Engine;
};

} // namespace R3