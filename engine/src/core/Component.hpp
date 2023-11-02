#pragma once

namespace R3 {

struct ComponentBase {
    ComponentBase() = default;
    virtual ~ComponentBase() {}
};

template <typename T>
struct Component final : public ComponentBase {
    std::vector<T> components;

    static Component<T>* cast(ComponentBase* ptr) {
        return reinterpret_cast<Component<T>*>(ptr);
    }
};

} // namespace R3