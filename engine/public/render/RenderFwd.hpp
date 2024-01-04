#pragma once

/// @brief Forward Declarations of render objects

namespace R3 {

class R3_API Window;
class R3_API Instance;
class R3_API Surface;
class R3_API PhysicalDevice;
class R3_API LogicalDevice;
class R3_API Queue;
class R3_API Swapchain;
class R3_API Image;
class R3_API ImageView;
class R3_API RenderPass;
class R3_API DescriptorPool;
class R3_API DescriptorSetLayout;
class R3_API DescriptorSet;
class R3_API PipelineLayout;
class R3_API GraphicsPipeline;
class R3_API Shader;
class R3_API Framebuffer;
class R3_API CommandPool;
class R3_API CommandBuffer;
class R3_API Semaphore;
class R3_API Fence;
class R3_API DeviceMemory;
class R3_API Buffer;
struct R3_API Vertex;
struct R3_API VertexBindingSpecification;
struct R3_API VertexAttributeSpecification;
class R3_API VertexBuffer;
template <std::integral T>
class R3_API IndexBuffer;
class R3_API UniformBuffer;
class R3_API StorageBuffer;
class R3_API TextureBuffer;
class R3_API Sampler;
class R3_API ColorBuffer;
class R3_API DepthBuffer;

} // namespace R3