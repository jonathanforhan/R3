#include "Scene.hpp"

namespace R3 {

Scene& Scene::instance() {
    static Scene _instance;
    return _instance;
}

} // namespace R3
