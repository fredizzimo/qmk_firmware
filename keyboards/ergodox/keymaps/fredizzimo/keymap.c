#include "ergodox.h"
#include "debug.h"
#include "action_layer.h"
#include "keymap_sv_fi.h"
#include "quantum_keycodes.h"

enum layers {
	BASE,
	SYM1,
    SYM2,
    NAV,
};

enum my_keycodes {
    APQU=SAFE_RANGE, // '"
    TILD, // ~
    CIRC, // ^
    ACUT, // ´
    GRAV, // `
};

#define LS LSFT_T
#define LC LCTL_T
#define LG LGUI_T
#define RS LSFT_T
#define RC RCTL_T
#define RG RGUI_T
#define AG ALGR_T
#define AL ALT_T
#define ME MEH_T

#define OS_LSFT OSM(MOD_LSFT)
#define OS_RSFT OSM(MOD_RSFT)
#define OS_LCTL OSM(MOD_LCTL)
#define OS_RCTL OSM(MOD_RCTL)
#define OS_LALT OSM(MOD_LALT)
#define OS_RALT OSM(MOD_RALT)
#define OS_LGUI OSM(MOD_LGUI)
#define OS_RGUI OSM(MOD_RGUI)
#define OS_MEH OSM(MOD_MEH)
#define OS_SYM1 OSL(SYM1)
#define OS_SYM2 OSL(SYM2)
#define LA_NAV TG(NAV)

#define L_TAB LSFT(KC_TAB)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
// Keymap 0: Basic layer
[BASE] = KEYMAP(
        // left hand
        KC_MPLY, KC_F1, KC_F2, KC_F3,  KC_F4, KC_F5, KC_F6,
        OS_MEH,  SV_Q,  SV_W,  SV_E,   SV_R,  SV_T,  KC_NO,
        KC_ESC,  SV_A,  SV_S,  SV_D,   SV_F,  SV_G,
        OS_LCTL, SV_Z,  SV_X,  SV_C,   SV_V,  SV_B,  KC_NO,
        OS_LCTL, KC_NO, KC_NO, KC_MENU, KC_DEL,
                                              KC_NO, KC_NO,
                                                     KC_NO,
                                  OS_LSFT, OS_SYM1, LA_NAV,
        // right hand
        KC_F7, KC_F8, KC_F9,   KC_F10,  KC_F11,  KC_F12, KC_PSCR,
        KC_NO, SV_Y,  SV_U,    SV_I,    SV_O,    SV_P,   SV_AA,
               SV_H,  SV_J,    SV_K,    SV_L,    SV_OE,  SV_AE,
        KC_NO, SV_N,  SV_M,    SV_COMM, SV_DOT,  APQU,   OS_LALT,
                      KC_BSPC, OS_LGUI, KC_NO,   KC_NO,  KC_NO,
        KC_NO,  KC_NO,
        KC_NO,
        KC_ENT, OS_SYM2, KC_SPACE
    ),
[SYM1] = KEYMAP(
        // left hand
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_TRNS, SV_PND,  SV_LCBR, SV_RCBR, SV_DLR,  CIRC,    KC_NO,
        KC_TRNS, SV_UNDS, SV_LPRN, SV_RPRN, SV_LBRC, SV_RBRC,
        KC_TRNS, SV_BSLS, GRAV,    ACUT,    SV_LT,   SV_GT,   KC_NO,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                                                  KC_CAPS, KC_PAUSE,
                                                            KC_TRNS,
                                         KC_TRNS, KC_TRNS,  KC_TRNS,
        // right hand
        KC_NO, KC_NO,   KC_NO,   KC_NO,   KC_NO,  KC_NO,   KC_NO,
        KC_NO, SV_SLSH, SV_7,    SV_8,    SV_9,   TILD,    KC_NO,
               SV_ASTR, SV_4,    SV_5,    SV_6,   SV_PLUS, SV_EQL,
        KC_NO, SV_0,    SV_1,    SV_2,    SV_3,   SV_MINS, KC_TRNS,
                        KC_TRNS, SV_COMM, SV_DOT, KC_NO,   KC_NO,
        KC_SLCK, KC_INS,
        KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS
    ),
[SYM2] = KEYMAP(
        // left hand
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_TRNS, SV_CURR, SV_HASH, SV_EURO, SV_PERC, SV_CIRC, KC_NO,
        KC_TRNS, SV_AT,   SV_AMPR, SV_PIPE, SV_QUES, SV_EXLM,
        KC_TRNS, KC_NO,   SV_GRAV, SV_ACUT, L_TAB,   KC_TAB, KC_NO,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                                                      DEBUG, RESET,
                                                           KC_TRNS,
                                         KC_TRNS, KC_TRNS, KC_TRNS,
        // right hand
        KC_NO, KC_NO,  KC_NO,   KC_NO,   KC_NO, KC_NO,   KC_NO,
        KC_NO, KC_F12, KC_F7,   KC_F8,   KC_F9, SV_TILD, KC_NO,
               KC_F11, KC_F4,   KC_F5,   KC_F6, SV_UMLT, KC_NO,
        KC_NO, KC_F10, KC_F1,   KC_F2,   KC_F3, SV_HALF, KC_TRNS,
                       KC_TRNS, KC_TRNS, KC_NO, KC_NO,   KC_NO,
        KC_TRNS, KC_TRNS,
        KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS
    ),
[NAV] = KEYMAP(
        // left hand
        KC_NO, KC_NO,   KC_NO,   KC_NO,   KC_NO,    KC_NO,  KC_NO,
        KC_NO, KC_NO,   KC_PGDN, KC_UP,   KC_PGDN,  KC_NO,  KC_NO,
        KC_NO, KC_HOME, KC_LEFT, KC_DOWN, KC_RIGHT, KC_END,
        KC_NO, KC_NO,   KC_NO,   KC_NO,   KC_NO,    KC_NO,  KC_NO,
        KC_NO, KC_NO,   KC_NO,   KC_NO,   KC_NO,
                                                 KC_TRNS, KC_TRNS,
                                                          KC_TRNS,
                                        KC_TRNS, KC_TRNS, KC_TRNS,
        // right hand
        KC_NO, KC_NO,   KC_NO,   KC_NO,   KC_NO,    KC_NO, KC_NO,
        KC_NO, KC_HOME, KC_PGDN, KC_PGUP, KC_END,   KC_NO, KC_NO,
               KC_LEFT, KC_DOWN, KC_UP,   KC_RIGHT, KC_NO, KC_NO,
        KC_NO, KC_NO,   KC_NO,   KC_NO,   KC_NO,    KC_NO, KC_NO,
                        KC_NO,   KC_NO,   KC_NO,    KC_NO, KC_NO,
        KC_TRNS, KC_TRNS,
        KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS
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

bool override_key(keyrecord_t* record, uint16_t normal, uint16_t shifted) {
    const uint8_t shift = MOD_BIT(KC_LSFT) | MOD_BIT(KC_RSFT);
    // Todo, should maybe also check at least the weak mods
    uint8_t current_mods = get_mods();
    if (record->event.pressed) {
        // Todo share this code with send keyboard report
#ifndef NO_ACTION_ONESHOT
        if (get_oneshot_mods()) {
#if (defined(ONESHOT_TIMEOUT) && (ONESHOT_TIMEOUT > 0))
            if (has_oneshot_mods_timed_out()) {
                dprintf("Oneshot: timeout\n");
                clear_oneshot_mods();
            }
#endif
            current_mods |= get_oneshot_mods();
            clear_oneshot_mods();
        }
#endif
        bool shift_pressed = current_mods & shift;
        const uint16_t target = shift_pressed ? shifted : normal;
        uint8_t keycode = target & 0xFF;
        if (keycode == KC_NO) {
            return false;
        }
        press_key_with_level_mods(target);
    }
    return false;
}

static bool wake_dead_key(uint16_t keycode, keyrecord_t* record) {
    if (record->event.pressed) {
        register_code16(keycode);
        unregister_code16(keycode);
        register_code16(KC_SPACE);
        unregister_code16(KC_SPACE);
    }
    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch(keycode) {
    case APQU:
        return override_key(record, SV_APOS, SV_QUOT);
    case TILD:
        return wake_dead_key(SV_TILD, record);
    case CIRC:
        return wake_dead_key(SV_CIRC, record);
    case ACUT:
        return wake_dead_key(SV_ACUT, record);
    case GRAV:
        return wake_dead_key(SV_GRAV, record);
    }
    return true;
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
