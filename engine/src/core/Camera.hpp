#pragma once
#include "api/Types.hpp"

namespace R3 {

class Camera {
public:
    Camera() = default;

    void translate_forward(float magnitude);
    void translate_backward(float magnitude);
    void translate_left(float magnitude);
    void translate_right(float magnitude);
    void translate_up(float magnitude);
    void translate_down(float magnitude);
    void look_around(float x, float y);
    void apply(mat4* view, mat4* projection, float aspect_ratio);

    const vec3& position() const { return _position; }

public:
    bool attached = false;

private:
    vec3 _position{0.0f, 1.0f, -60.0f};
    vec3 _front{0.0f, 0.0f, 1.0f};
    vec3 _up{0.0f, 1.0f, 0.0f};

    float _yaw{90};
    float _pitch{0};
    float _fov{45};
};

} // namespace R3