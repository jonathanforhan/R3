#pragma once

#include "api/Types.hpp"
#include "input/Event.hpp"
#include "input/InputCodes.hpp"

namespace R3 {

struct KeyboardEventPayload {
    Key key;
    InputModifier::Mask modifiers;
};

using KeyPressEvent = EVENT("on-key-press", KeyboardEventPayload);

using KeyRepeatEvent = EVENT("on-key-repeat", KeyboardEventPayload);

using KeyReleaseEvent = EVENT("on-key-release", KeyboardEventPayload);

} // namespace R3