#if not _WIN32
#error "Only Windows support currently"
#endif

#include <windows.h>
#include <R3>
#include "components/TransformComponent.hpp"
#include "core/Engine.hpp"
#include "core/Scene.hpp"

using VulkanMain = void*(__cdecl*)();

using namespace R3;

int main() {
    HMODULE hmod = LoadLibraryEx(TEXT("vulkan.dll"), NULL, 0);

    if (!hmod)
        return -1;

    VulkanMain vulkanMain = (VulkanMain)GetProcAddress(hmod, "VulkanMain");

    if (!vulkanMain)
        return -2;

    CurrentScene = reinterpret_cast<Scene*>(vulkanMain());
    if (CurrentScene->id == HASH32("foobar"))
        LOG(Info, "ID Success");

    CurrentScene->componentForEach([](TransformComponent& t) {
        for (auto i = 0; i < TransformComponent::length(); i++) {
            LOG(Info, t[i].x, t[i].y, t[i].z, t[i].w);
        }
    });

        return FreeLibrary(hmod)
        ? 0
        : -3;
}