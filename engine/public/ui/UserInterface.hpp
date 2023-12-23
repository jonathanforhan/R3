#pragma once

#include "render/RenderFwd.hpp"

namespace R3 {

class UserInterface {
private:
    UserInterface();
    UserInterface(const UserInterface&) = delete;
    UserInterface(UserInterface&&) = delete;
    ~UserInterface();

private:
    Ref<void> m_descriptorPool;
    friend class Engine;
};

} // namespace R3
