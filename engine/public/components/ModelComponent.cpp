#include "ModelComponent.hpp"
#include "detail/media/ModelImporter.hxx"

namespace R3 {

ModelComponent::ModelComponent(const std::string& path, Shader& shader)
    : m_shader(shader),
      m_directory(),
      m_file() {
    ModelImporter importer(path);
}

} // namespace R3