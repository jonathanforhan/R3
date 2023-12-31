#include "components/ModelComponent.hpp"

#include "core/Engine.hpp"
#include "core/Scene.hpp"
#include "render/Renderer.hxx"

namespace R3 {

ModelComponent::ModelComponent(const std::string& path) {
    reinterpret_cast<Renderer*>(EngineInstance->renderView().handle())->modelLoader().load(path, *this);
}

ModelComponent::~ModelComponent() {
    for (auto& mesh : m_meshes) {
        mesh.destroy();
    }
}

} // namespace R3