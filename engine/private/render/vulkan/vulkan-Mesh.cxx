#if R3_VULKAN

#include "render/Mesh.hpp"

#include <vulkan/vulkan.hpp>

namespace R3 {

Mesh::Mesh(std::span<Vertex> vertices, std::span<uint32> indices) {

}

Mesh::~Mesh() {

}

void Mesh::bind() {
}

} // namespace R3

#endif // R3_VULKAN