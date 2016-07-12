#include "ergodox.h"
#include "debug.h"
#include "action_layer.h"
#include "keymap_nordic.h"

#define BASE 0 // default layer

#define NAVLAYER KC_NO
#define LCTRLLAYER KC_NO
#define SYMLAYER KC_NO
#define SHIFTLAYER KC_NO
#define LEADER KC_NO

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* Keymap 0: Basic layer
 *
 * ,--------------------------------------------------.           ,--------------------------------------------------.
 * |   =    |   1  |   2  |   3  |   4  |   5  | LEFT |           | RIGHT|   6  |   7  |   8  |   9  |   0  |   -    |
 * |--------+------+------+------+------+-------------|           |------+------+------+------+------+------+--------|
 * | Del    |   Q  |   W  |   E  |   R  |   T  |  L1  |           |  L1  |   Y  |   U  |   I  |   O  |   P  |   \    |
 * |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
 * | BkSp   |   A  |   S  |   D  |   F  |   G  |------|           |------|   H  |   J  |   K  |   L  |; / L2|' / Cmd |
 * |--------+------+------+------+------+------| Hyper|           | Meh  |------+------+------+------+------+--------|
 * | LShift |Z/Ctrl|   X  |   C  |   V  |   B  |      |           |      |   N  |   M  |   ,  |   .  |//Ctrl| RShift |
 * `--------+------+------+------+------+-------------'           `-------------+------+------+------+------+--------'
 *   |Grv/L1|  '"  |AltShf| Left | Right|                                       |  Up  | Down |   [  |   ]  | ~L1  |
 *   `----------------------------------'                                       `----------------------------------'
 *                                        ,-------------.       ,-------------.
 *                                        | App  | LGui |       | Alt  |Ctrl/Esc|
 *                                 ,------|------|------|       |------+--------+------.
 *                                 |      |      | Home |       | PgUp |        |      |
 *                                 | Space|Backsp|------|       |------|  Tab   |Enter |
 *                                 |      |ace   | End  |       | PgDn |        |      |
 *                                 `--------------------'       `----------------------'
 */
[BASE] = KEYMAP(  // layer 0 : default
        // left hand
        KC_NO,   KC_F1,    KC_F2,   KC_F3, KC_F4,     KC_F5,      KC_F6,
        NO_ALGR, NAVLAYER, KC_W,    KC_E,  KC_R,      KC_T,       LSFT(KC_TAB),
        KC_LALT, KC_Q,     KC_S,    KC_D,  KC_F,      KC_G,
        NO_QUES, KC_A,     KC_X,    KC_C,  KC_V,      KC_B,       KC_ESC,
        KC_LEFT, KC_Z,     KC_DOWN, KC_UP, KC_RIGHT,
                                                      KC_NO,      RESET,
                                                                  KC_LGUI,
                                            SYMLAYER, SHIFTLAYER, LEADER,
        // right hand
        KC_F7,     KC_F8,    KC_F9,   KC_F10,    KC_F11,  KC_F12,  KC_NO,
        KC_TAB,    KC_Y,     KC_U,    KC_I,      KC_O,    KC_NO,   KC_NO,
                   KC_H,     KC_J,    KC_K,      KC_L,    KC_P,    KC_DEL,
        KC_RETURN, KC_N,     KC_M,    KC_COMM,   KC_DOT,  SV_OUML, SV_AUML,
                             KC_HOME, KC_PGDOWN, KC_PGUP, SV_QUOT, SV_ARING,
        KC_NO,     KC_NO,
        KC_APP,
        KC_LCTRL,  KC_SPACE, KC_BSPACE
    ),

};

const uint16_t PROGMEM fn_actions[] = {
};

// Runs just one time when the keyboard initializes.
void matrix_init_user(void) {

};

// Runs constantly in the background, in a loop.
void matrix_scan_user(void) {

    uint8_t layer = biton32(layer_state);

    ergodox_board_led_off();
    ergodox_right_led_1_off();
    ergodox_right_led_2_off();
    ergodox_right_led_3_off();
    switch (layer) {
      // TODO: Make this relevant to the ErgoDox EZ.
        case 1:
            ergodox_right_led_1_on();
            break;
        case 2:
            ergodox_right_led_2_on();
            break;
        default:
            // none
            break;
    }

};
