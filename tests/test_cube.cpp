#include "api/Check.hpp"
#include "api/Log.hpp"
#include "api/Math.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"

using namespace R3;

/*
Vertex vertices[] = {
    // positions         colors              texture coords
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},   // top right
    {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // bottom right
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
    {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // top left

    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},   // top right
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // bottom right
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // top left

    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},   // top right
    {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // bottom right
    {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
    {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // top left

    {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},   // top right
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // bottom right
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
    {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // top left

    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},   // top right
    {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // bottom right
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
    {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // top left

    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},   // top right
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // bottom right
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
    {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // top left
};
uint32 indices[] = {
    0,  1,  3,  1,  2,  3,  4,  5,  7,  5,  6,  7,  8,  9,  11, 9,  10, 11,
    12, 13, 15, 13, 14, 15, 16, 17, 19, 17, 18, 19, 20, 21, 23, 21, 22, 23,
};
*/

class MyEnt : public Entity {
public:
    MyEnt(int x) : x(x) { LOG(Info, "CON", x); }
    ~MyEnt() { LOG(Info, "DES"); }
    int x;
};

int main(void) {
#if defined _DEBUG || !defined NDEBUG
    LOG(Info, "Running Debug Cube Test...");
#elif
    LOG(Info, "Running Release Cube Test...");
#endif

    struct Point {
        int x = 1, y = 2;
    };

    MyEnt& ent = Entity::create<MyEnt>(4);
    ent.add_component<Point>();

    auto& point = ent.get_component<Point>();

    LOG(Info, point.x, point.y);
}
