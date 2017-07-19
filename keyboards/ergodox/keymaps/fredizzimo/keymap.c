#include "ergodox.h"
#include "debug.h"
#include "action_layer.h"
#include "keymap_sv_fi.h"
#include "quantum_keycodes.h"

enum layers {
	BASE,
	LSYM1,
    RSYM1,
    LSYM2,
    RSYM2,
    LNAV,
    RNAV,
};

#define NAVLAYER KC_NO
#define LCTRLLAYER KC_NO

enum my_keycodes {
	UNAM=SAFE_RANGE, // _&|
    EUPO, // €£¤
    APQU, // '"
    EXQU, // !?
    TILD, // ~
    CIRC, // ^
    ACUT, // ´
    GRAV, // `
    NAV,
    SYM,
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
#define LS1(kc) LT(LSYM1, kc)
#define RS1(kc) LT(RSYM1, kc)
#define LS2(kc) LT(LSYM2, kc)
#define RS2(kc) LT(RSYM2, kc)
#define LN(kc) LT(LNAV, kc)
#define RN(kc) LT(RNAV, kc)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
// Keymap 0: Basic layer
[BASE] = KEYMAP(
        // left hand
        KC_MPLY, KC_F1, KC_F2,    KC_F3,     KC_F4,    KC_F5,     KC_F6,
        KC_NO,   SV_Q,  LG(SV_W), ME(SV_E),  AG(SV_R), SV_T,      NAV,
        UNAM,    SV_A,  AL(SV_S), LC(SV_D),  LS(SV_F), LS1(SV_G),
        EUPO,    SV_Z,  SV_X,     SV_C,      SV_V,     LS2(SV_B), KC_NO,
        KC_LCTL, KC_NO, KC_NO,    KC_NO,     KC_DEL,
                                                           KC_NO, KC_NO,
                                                                  KC_NO,
                                                KC_ESC, KC_ENT, KC_LGUI,
        // right hand
        KC_F7, KC_F8,     KC_F9,    KC_F10,    KC_F11,   KC_F12, KC_PSCR,
        SYM,   SV_Y,      AG(SV_U), ME(SV_I),  RG(SV_O), SV_P,   SV_AA,
               RS1(SV_H), RS(SV_J), RC(SV_K),  AL(SV_L), SV_OE,  SV_AE,
        KC_NO, RS2(SV_N), SV_M,     SV_COMM,   SV_DOT,   APQU,   EXQU,
                          KC_BSPC,  KC_LEFT,   KC_DOWN,  KC_UP,  KC_RIGHT,
        KC_NO,   KC_NO,
        KC_NO,
        KC_MENU, KC_TAB, KC_SPACE
    ),
[LSYM1] = KEYMAP(
        // left hand
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                                                     KC_TRNS, KC_TRNS,
                                                              KC_TRNS,
                                            KC_TRNS, KC_TRNS, KC_TRNS,
        // right hand
        KC_NO, KC_NO,   KC_NO, KC_NO,   KC_NO,  KC_NO,   KC_NO,
        KC_NO, SV_PERC, SV_7,  SV_8,    SV_9,   TILD,    KC_NO,
               SV_ASTR, SV_4,  SV_5,    SV_6,   SV_PLUS, SV_EQL,
        KC_NO, SV_SLSH, SV_1,  SV_2,    SV_3,   SV_MINS, SV_SECT,
                        SV_0,  SV_COMM, SV_DOT, KC_NO,   KC_NO,
        KC_SLCK, KC_INS,
        KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS
    ),
[RSYM1] = KEYMAP(
        // left hand
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,  KC_NO, KC_NO,
        KC_NO,   SV_AT,   SV_LCBR, SV_RCBR, SV_DLR, CIRC,  KC_NO,
        SV_PIPE, SV_HASH, SV_LPRN, SV_RPRN, SV_LT,  SV_GT,
        SV_CURR, SV_BSLS, SV_LBRC, SV_RBRC, GRAV,   ACUT,  KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
                                                KC_CAPS, KC_PAUSE,
                                                          KC_TRNS,
                                       KC_TRNS, KC_TRNS,  KC_TRNS,
        // right hand
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS,
        KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS
    ),
[LSYM2] = KEYMAP(
        // left hand
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                                                      DEBUG,    RESET,
                                                              KC_TRNS,
                                            KC_TRNS, KC_TRNS, KC_TRNS,
        // right hand
        KC_NO, KC_NO,  KC_NO, KC_NO, KC_NO, KC_NO,   KC_NO,
        KC_NO, KC_F12, KC_F7, KC_F8, KC_F9, SV_TILD, KC_NO,
               KC_F11, KC_F4, KC_F5, KC_F6, SV_UMLT, KC_NO,
        KC_NO, KC_F10, KC_F1, KC_F2, KC_F3, SV_HALF, KC_NO,
                       KC_NO, KC_NO, KC_NO, KC_NO,   KC_NO,
        KC_TRNS, KC_TRNS,
        KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS
    ),
[RSYM2] = KEYMAP(
        // left hand
        KC_NO, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO, KC_NO,   KC_MUTE, KC_VOLD, KC_VOLU, SV_CIRC, KC_NO,
        KC_NO, KC_MPLY, KC_MSTP, KC_MPRV, KC_MNXT, SV_GT,
        KC_NO, KC_NO,   KC_NO,   KC_NO,   SV_GRAV, SV_ACUT, KC_NO,
        KC_NO, KC_NO,   KC_NO,   KC_NO,   KC_NO,
                                                     DEBUG, RESET,
                                                          KC_TRNS,
                                        KC_TRNS, KC_TRNS, KC_TRNS,
        // right hand
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS,
        KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS
    ),
[LNAV] = KEYMAP(
        // left hand
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
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
[RNAV] = KEYMAP(
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
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
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
    if (record->event.pressed) {
        bool shift_pressed = keyboard_report->mods & shift;
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
        register_code(keycode);
        unregister_code(keycode);
        register_code(KC_SPACE);
        unregister_code(KC_SPACE);
    }
    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch(keycode) {
    case EUPO:
        return override_key(record, SV_EURO, SV_PND);
    case APQU:
        return override_key(record, SV_APOS, SV_QUOT);
    case EXQU:
        return override_key(record, SV_EXLM, SV_QUES);
    case TILD:
        return wake_dead_key(SV_TILD, record);
    case CIRC:
        return wake_dead_key(SV_CIRC, record);
    case ACUT:
        return wake_dead_key(SV_ACUT, record);
    case GRAV:
        return wake_dead_key(SV_GRAV, record);
    case NAV:
        return true;
    case SYM:
        return true;
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
