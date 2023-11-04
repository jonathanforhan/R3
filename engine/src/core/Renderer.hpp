#pragma once
#include "api/Types.hpp"

namespace R3 {

class Renderer {
private:
    Renderer();

public:
    ~Renderer();

    void render();

private:
    friend class Engine;
};

} // namespace R3