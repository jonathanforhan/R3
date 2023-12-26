#pragma once

#include "api/Construct.hpp"
#include "api/Ref.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

class UserInterface {
public:
    UserInterface();
    NO_COPY(UserInterface);
    NO_MOVE(UserInterface);
    ~UserInterface();

    static void newFrame();
    static void draw(const CommandBuffer& commandBuffer);

private:
    Ref<void> m_descriptorPool;
};

} // namespace R3
