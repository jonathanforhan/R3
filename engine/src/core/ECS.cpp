#include "ECS.hpp"

namespace R3 {

ECS& ECS::instance() {
    static ECS _instance;
    return _instance;
}

} // namespace R3