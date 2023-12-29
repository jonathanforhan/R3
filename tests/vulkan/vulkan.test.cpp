#include <R3>
#include "components/TransformComponent.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "core/Scene.hpp"
#include "input/WindowEvent.hpp"

using namespace R3;

struct FooBar : public Entity {};

extern "C" {

R3_DLL void* VulkanMain() {
    static Scene scene = Scene(HASH32("foobar"));
    CurrentScene = &scene;

    auto& entity = Entity::create<FooBar>();
    auto& transform = entity.get<TransformComponent>();
    transform *= 14;

    Scene::bindEventListener([](const WindowResizeEvent& e) { LOG(Info, e.payload.width, e.payload.height); });

    return CurrentScene;
}

} // extern "C"