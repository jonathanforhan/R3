#pragma once
#include "System.hpp"
#include "core/Mesh.hpp"

namespace R3 {

class CubeMapSystem : public System {
public:
    CubeMapSystem();

    void tick(double dt) override;

private:
    Mesh m_cubeMesh;
};

} // namespace R3 {
