#pragma once
#include "api/Types.hpp"

namespace R3 {

enum class RenderPrimitive {
    Triangles,
    Lines,
    Points,
};

enum class StencilFunction {
    Always,
    Equal,
    NotEqual,
};

enum class StencilOperation {
    Keep,
    Zero,
    Replace,
    Increment,
    IncrementWrap,
    Decrement,
    DecrementWrap,
    Invert,
};

class Renderer {
private:
    Renderer();
    friend class Engine; // friend so engine can construct

public:
    ~Renderer();

    void enableCulling();
    void enableDepthTest();
    void enableStencilTest();
    void disableCulling();
    void disableDepthTest();
    void disableStencilTest();
    void stencilFunction(StencilFunction func, int32 ref, uint8 mask);
    void stencilMask(uint8 mask);
    void stencilOperation(StencilOperation stencilFail, StencilOperation depthFail, StencilOperation pass);

    void predraw() const;
    void drawElements(RenderPrimitive primitive, uint32 indiceCount) const;
    void drawArrays(RenderPrimitive primitive, uint32 vertexCount) const;
};

} // namespace R3