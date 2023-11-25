#pragma once
#include "api/Types.hpp"
#include "render/Instance.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/Pipeline.hpp"
#include "render/Queues.hpp"

namespace R3 {

class Renderer {
protected:
    Renderer();
    friend class Engine;

public:
    Renderer(const Renderer&) = delete;
    void operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = default;
    Renderer& operator=(Renderer&&) = default;

    ~Renderer();

private:
    Instance m_instance;
    PhysicalDevice m_physicalDevice;
    LogicalDevice m_logicalDevice;
    Pipeline m_pipeline;
};

} // namespace R3