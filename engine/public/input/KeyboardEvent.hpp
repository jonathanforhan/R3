#pragma once

#include "api/Types.hpp"
#include "input/Event.hpp"
#include "input/KeyCodes.hpp"

namespace R3 {

struct KeyboardEventPayload {
    Key key;
    KeyModifier::Mask modifiers;
};

using KeyPressEvent = EVENT("on-key-press", KeyboardEventPayload);
using KeyReleaseEvent = EVENT("on-key-release", KeyboardEventPayload);

} // namespace R3