#pragma once
#include <cstdint>
#include <glm/glm.hpp>

namespace R3 {

// vecs are first class citizens
using glm::vec1;
using glm::vec2;
using glm::vec3;
using glm::vec4;

using glm::ivec1;
using glm::ivec2;
using glm::ivec3;
using glm::ivec4;

using glm::uvec1;
using glm::uvec2;
using glm::uvec3;
using glm::uvec4;

using glm::fvec1;
using glm::fvec2;
using glm::fvec3;
using glm::fvec4;

using glm::dvec1;
using glm::dvec2;
using glm::dvec3;
using glm::dvec4;

// mats are first class citizens
using glm::mat2;
using glm::mat3;
using glm::mat4;

using glm::fmat2;
using glm::fmat3;
using glm::fmat4;

using glm::dmat2;
using glm::dmat3;
using glm::dmat4;

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
#if SIZE_MAX == UINT_MAX
using isize = int;
#elif SIZE_MAX == ULONG_MAX
using isize = long;
#elif SIZE_MAX == ULLONG_MAX
using isize = long long;
#endif

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using usize = size_t;

enum class InputAction : int8 {
    Release = 0,
    Press = 1,
    Repeat = 2,
};

enum class MouseButton : int8 {
    Left = 0,
    Right = 1,
    Middle = 2,
};

enum class Key : int16 {
    Unknown = -1,
    Space = 32,
    Apostrophe = 39,
    Comma = 44,
    Minus = 45,
    Period = 46,
    Slash = 47,
    Key_0 = 48,
    Key_1 = 49,
    Key_2 = 50,
    Key_3 = 51,
    Key_4 = 52,
    Key_5 = 53,
    Key_6 = 54,
    Key_7 = 55,
    Key_8 = 56,
    Key_9 = 57,
    Semicolon = 59,
    Equal = 61,
    Key_A = 65,
    Key_B = 66,
    Key_C = 67,
    Key_D = 68,
    Key_E = 69,
    Key_F = 70,
    Key_G = 71,
    Key_H = 72,
    Key_I = 73,
    Key_J = 74,
    Key_K = 75,
    Key_L = 76,
    Key_M = 77,
    Key_N = 78,
    Key_O = 79,
    Key_P = 80,
    Key_Q = 81,
    Key_R = 82,
    Key_S = 83,
    Key_T = 84,
    Key_U = 85,
    Key_V = 86,
    Key_W = 87,
    Key_X = 88,
    Key_Y = 89,
    Key_Z = 90,
    LeftBracket = 91,
    Backslash = 92,
    RightBracket = 93,
    GraveAccent = 96,
    World1 = 161,
    World2 = 162,
    Escape = 256,
    Enter = 257,
    Tab = 258,
    Backspace = 259,
    Insert = 260,
    Delete = 261,
    Right = 262,
    Left = 263,
    Down = 264,
    Up = 265,
    PageUp = 266,
    PageDown = 267,
    Home = 268,
    End = 269,
    CapsLock = 280,
    ScrollLock = 281,
    NumLock = 282,
    PrintScreen = 283,
    Pause = 284,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    F13 = 302,
    F14 = 303,
    F15 = 304,
    F16 = 305,
    F17 = 306,
    F18 = 307,
    F19 = 308,
    F20 = 309,
    F21 = 310,
    F22 = 311,
    F23 = 312,
    F24 = 313,
    F25 = 314,
    KP_0 = 320,
    KP_1 = 321,
    KP_2 = 322,
    KP_3 = 323,
    KP_4 = 324,
    KP_5 = 325,
    KP_6 = 326,
    KP_7 = 327,
    KP_8 = 328,
    KP_9 = 329,
    KP_Decimal = 330,
    KP_Divide = 331,
    KP_Multiply = 332,
    KP_Subtract = 333,
    KP_Add = 334,
    KP_Enter = 335,
    KP_Equal = 336,
    LeftShift = 340,
    LeftControl = 341,
    LeftAlt = 342,
    LeftSuper = 343,
    RightShift = 344,
    RightControl = 345,
    RightAlt = 346,
    RightSuper = 347,
    Menu = 348,
};

} // namespace R3
