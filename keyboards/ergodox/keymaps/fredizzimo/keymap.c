#include "ergodox.h"
#include "debug.h"
#include "action_layer.h"
#include "keymap_sv_fi.h"
#include "quantum_keycodes.h"

enum layers {
	BASE,
	SYMBOLS,
};

#define NAVLAYER KC_NO
#define LCTRLLAYER KC_NO

enum my_keycodes {
	LEFT_TAB=SAFE_RANGE,
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
        NAVLAYER,     KC_F1,   KC_F2, KC_F3,   KC_F4,    KC_F5, KC_F6,
        M(EURO_PND),  SV_Q,    SV_W,  SV_E,    SV_R,     SV_T,  M(RIGHT_TAB),
        KC_ESC,       SV_A,    SV_S,  SV_D,    SV_F,     SV_G,
        M(QUES_EXLM), SV_Z,    SV_X,  SV_C,    SV_V,     SV_B,  KC_DEL,
        KC_LCTRL,     KC_LEFT, KC_UP, KC_DOWN, KC_RIGHT,
                                                                KC_NO,   KC_NO,
                                                                       KC_LGUI,
                                                  MO(SYMBOLS), KC_LSFT,  KC_MEH,
        // right hand
        KC_F7,       KC_F8, KC_F9,   KC_F10,  KC_F11, KC_F12,       KC_NO,
        M(LEFT_TAB), SV_Y,  SV_U,    SV_I,    SV_O,   SV_P,         SV_AA,
                     SV_H,  SV_J,    SV_K,    SV_L,   SV_OE,        SV_AE,
        KC_BSPACE,   SV_N,  SV_M,    SV_COMM, SV_DOT, M(APOS_QUOT), KC_ENTER,
                            KC_LEFT, KC_DOWN, KC_UP,  KC_RIGHT,     KC_NO,
        KC_NO,    KC_NO,
        KC_MENU,
        KC_LALT, KC_SPACE, KC_LCTRL
    ),
[SYMBOLS] = KEYMAP(
        // left hand
        KC_NO,   KC_NO,   SV_UMLT, SV_GRAV, SV_ACUT, SV_CIRC, KC_NO,
        SV_PIPE, SV_AT,   SV_LCBR, SV_RCBR, SV_DLR,  SV_CIRC, M(RIGHT_TAB),
        SV_UNDS, SV_HASH, SV_LPRN, SV_RPRN, SV_LT,   SV_GT,
        SV_AMPR, SV_BSLS, SV_LBRC, SV_RBRC, SV_GRAV, SV_ACUT, KC_DEL,
        KC_NO,   KC_NO, KC_NO,   KC_NO,   KC_NO,
                                                        KC_NO, RESET,
                                                               KC_NO,
                                               KC_TRNS, KC_NO, KC_NO,
        // right hand
        KC_NO,       KC_NO,   SV_TILD, SV_SECT, SV_HALF, SV_CURR, KC_NO,
        M(LEFT_TAB), SV_PERC, SV_7,    SV_8,     SV_9,   SV_TILD, KC_NO,
                     SV_ASTR, SV_4,    SV_5,     SV_6,   SV_PLUS, SV_EQL,
        KC_BSPACE,   SV_SLSH, SV_1,    SV_2,     SV_3,   SV_MINS, KC_ENTER,
                              SV_0,    SV_COMM,  SV_DOT, KC_NO,   KC_NO,
        KC_NO,   KC_NO,
        KC_NO,
        KC_LALT, KC_SPACE, KC_LCTRL
    ),
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

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch(keycode) {
        case LEFT_TAB:
            override_key(record, LSFT(KC_TAB), KC_NO);
            break;
        case RIGHT_TAB:
            override_key(record, KC_TAB, KC_NO);
            break;
        case QUES_EXLM:
            override_key(record, SV_QUES, SV_EXLM);
            break;
        case EURO_PND:
            override_key(record, SV_EURO, SV_PND);
            break;
        case DIAE_TILD:
            override_key(record, SV_UMLT, SV_TILD);
            break;
        case HALF_SECT:
            override_key(record, SV_HALF, SV_SECT);
            break;
        case APOS_QUOT:
            override_key(record, SV_APOS, SV_QUOT);
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
