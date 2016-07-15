#include "ergodox.h"
#include "debug.h"
#include "action_layer.h"
#include "keymap_nordic.h"

enum layers {
	BASE,
	SYMBOLS,
};

#define NAVLAYER KC_NO
#define LCTRLLAYER KC_NO
#define LEADER KC_NO

enum macros {
	LEFT_TAB,
	RIGHT_TAB,
	QUES_EXLM,
	EURO_PND,
	DIAE_TILD,
	HALF_SECT,
	APOS_QUOT,
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
// Keymap 0: Basic layer
[BASE] = KEYMAP(
        // left hand
        KC_NO,        KC_F1,    KC_F2, KC_F3,        KC_F4,        KC_F5, KC_F6,
        NO_ALGR,      NAVLAYER, KC_W,  KC_E,         KC_R,         KC_T,  M(LEFT_TAB),
        KC_LALT,      KC_Q,     KC_S,  KC_D,         KC_F,         KC_G,
        M(QUES_EXLM), KC_A,     KC_X,  KC_C,         KC_V,         KC_B,  KC_ESC,
        M(EURO_PND),  KC_Z,     KC_NO, M(DIAE_TILD), M(HALF_SECT),
                                                                       KC_NO,   KC_NO,
                                                                              KC_LGUI,
                                                        MO(SYMBOLS), KC_LSFT,  LEADER,
        // right hand
        KC_F7,        KC_F8,KC_F9,   KC_F10,    KC_F11, KC_F12,       KC_NO,
        M(RIGHT_TAB), KC_Y, KC_U,    KC_I,      KC_O,   KC_NO,        KC_NO,
                      KC_H, KC_J,    KC_K,      KC_L,   KC_P,         KC_DEL,
        KC_BSPACE,    KC_N, KC_M,    KC_COMM,   KC_DOT, SV_OUML,      SV_AUML,
                            KC_NO,   KC_NO,     KC_NO,  M(APOS_QUOT), SV_ARING,
        KC_NO,    KC_NO,
        KC_NO,
        KC_LCTRL, KC_SPACE, KC_ENTER
    ),
[SYMBOLS] = KEYMAP(
        // left hand
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,   KC_NO,   NO_LCBR, NO_RCBR, NO_DLR,  NO_CIRC, KC_NO,
        KC_LALT, NO_AT,   NO_LPRN, NO_RPRN, NO_LESS, NO_GRTR,
        NO_AMP,  KC_HASH, NO_LBRC, NO_RBRC, NO_GRV,  NO_ACUT, KC_NO,
        NO_PIPE, NO_BSLS, KC_NO,   KC_NO,   KC_NO,
                                                        KC_NO, RESET,
                                                               KC_NO,
                                               KC_TRNS, KC_NO, KC_NO,
        // right hand
        KC_NO,     KC_NO,   KC_NO, KC_NO,    KC_NO,  KC_NO,   KC_NO,
        KC_NO,     KC_PERC, KC_7,  KC_8,     KC_9,   KC_NO,   KC_NO,
                   SV_ASTR, KC_4,  KC_5,     KC_6,   KC_NO,   KC_DEL,
        KC_BSPACE, NO_SLSH, KC_1,  KC_2,     KC_3,   NO_PLUS, NO_EQL,
                            KC_0,  KC_COMMA, KC_DOT, NO_MINS, NO_UNDS,
        KC_NO,    KC_NO,
        KC_NO,
        KC_LCTRL, KC_SPACE, KC_ENTER
    ),
};

const uint16_t PROGMEM fn_actions[] = {
};


// Runs just one time when the keyboard initializes.
void matrix_init_user(void) {

};

void press_key_with_level_mods(uint16_t key) {
    const uint8_t interesting_mods = MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT) | MOD_BIT(KC_RALT);

    // Save the state
    const uint8_t real_mods = get_mods();
    const uint8_t weak_mods = get_weak_mods();
    const uint8_t macro_mods = get_macro_mods();

    uint8_t target_mods = (key >> 8) & (QK_MODS_MAX >> 8);
    // The 5th bit indicates that it's a right hand mod,
    // which needs some fixup
    if (target_mods & 0x10) {
        target_mods &= 0xF;
        target_mods <<= 4;
    }

    // Clear the mods that we are potentially going to modify,
    del_mods(interesting_mods);
    del_weak_mods(interesting_mods);
    del_macro_mods(interesting_mods);

    // Enable the mods that we need
    add_mods(target_mods & interesting_mods);

    // Press and release the key
    register_code(key & 0xFF);
    unregister_code(key & 0xFF);

    // Restore the previous state
    set_mods(real_mods);
    set_weak_mods(weak_mods);
    set_macro_mods(macro_mods);
    send_keyboard_report();
}

void override_key(keyrecord_t* record, uint16_t normal, uint16_t shifted) {
    const uint8_t shift = MOD_BIT(KC_LSFT) | MOD_BIT(KC_RSFT);
    if (record->event.pressed) {
        bool shift_pressed = keyboard_report->mods & shift;
        const uint16_t target = shift_pressed ? shifted : normal;
        uint8_t keycode = target & 0xFF;
        if (keycode == KC_NO) {
            return;
        }
        press_key_with_level_mods(target);
    }
}

const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt) {
    switch(id) {
        case LEFT_TAB:
            override_key(record, LSFT(KC_TAB), KC_NO);
            break;
        case RIGHT_TAB:
            override_key(record, KC_TAB, KC_NO);
            break;
        case QUES_EXLM:
            override_key(record, NO_QUES, KC_EXLM);
            break;
        case EURO_PND:
            override_key(record, NO_EURO, NO_PND);
            break;
        case DIAE_TILD:
            override_key(record, SV_DIAE, NO_TILD);
            break;
        case HALF_SECT:
            override_key(record, SV_HALF, SV_SECT);
            break;
        case APOS_QUOT:
            override_key(record, NO_APOS, SV_QUOT);
    }
    return MACRO_NONE;
}

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
