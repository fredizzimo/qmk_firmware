#include "quantum.h"

// Use internal pull up resistor,
// so that you can tie directly to ground on the right hand side.
bool is_keyboard_left(void) {
#if defined(SPLIT_HAND_PIN)
    // Test pin SPLIT_HAND_PIN for High/Low, if low it's right hand
    setPinInputHigh(SPLIT_HAND_PIN);
#    ifdef SPLIT_HAND_PIN_LOW_IS_LEFT
    return !readPin(SPLIT_HAND_PIN);
#    else
    return readPin(SPLIT_HAND_PIN);
#    endif
#endif

    return is_keyboard_master();
}
