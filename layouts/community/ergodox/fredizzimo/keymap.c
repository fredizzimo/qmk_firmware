#include QMK_KEYBOARD_H
#include "debug.h"
#include "action_layer.h"
#include "keymap_sv_fi.h"
#include "quantum_keycodes.h"

enum layers {
    BASE,
    SWAP,
    NUM,
    SYM1,
    SYM2,
    SYM3,
    MEDIA,
    FKEYS,
    NAV,
};

enum my_keycodes {
    APQU=SAFE_RANGE, // '"
    TILD, // ~
    CIRC, // ^
    ACUT, // ´
    GRAV, // `
    BSPC_ENT,
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
#define OS_HYPR OSM(MOD_HYPR)
#define OS_SYM1 OSL(SYM1)
#define OS_SYM2 OSL(SYM2)
#define LA_NAV TG(NAV)

#define L_TAB LSFT(KC_TAB)
#define CTRL_BSPC LCTL(KC_BSPC)
#define SFT_ENT LSFT(KC_ENT)



#define ESC_SHIFT MT(MOD_LSFT, KC_ESC)
#define DEL_CTRL MT(MOD_LCTL, KC_DEL)
#define AE_SHIFT MT(MOD_RSFT, SV_AE)
#define BS_CTRL MT(MOD_RCTL, KC_BSPC)
#define SP_SYM LT(SYM, KC_SPACE)
#define ENT_NUM LT(NUM, KC_ENT)

#define VS_CMD OSM(MOD_LSFT | MOD_LCTL)

#define STEP_OUT LSFT(KC_F11)
#define STEP_IN KC_F11
#define STEP_OVER KC_F10


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
// Keymap 0: Basic layer
[BASE] = KEYMAP(
        // left hand
        KC_NO,      KC_NO,      CTRL_BSPC,  KC_BSPACE,  KC_DEL,     KC_NO,      KC_NO,
        KC_TAB,     SV_Q,       SV_W,       SV_E,       SV_R,       SV_T,       KC_NO,
        KC_ESC,     SV_A,       SV_S,       SV_D,       SV_F,       SV_G,
        KC_NO,      SV_Z,       SV_X,       SV_C,       SV_V,       SV_B,       KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      MO(NAV),
                                                                    KC_NO,      KC_NO,
                                                                                KC_NO,
                                                        KC_SPACE,   MO(SWAP),   KC_NO,
        // right hand
        KC_NO,      KC_NO,      CTRL_BSPC,  KC_BSPC,    KC_DEL,     KC_NO,      KC_NO,
        KC_NO,      SV_Y,       SV_U,       SV_I,       SV_O,       SV_P,       MO(FKEYS),
                    SV_H,       SV_J,       SV_K,       SV_L,       MO(NUM),    MO(SYM3),
        KC_NO,      SV_N,       SV_M,       SV_COMM,    SV_DOT,     MO(SYM1),   MO(SYM2),
                    MO(NAV),    KC_NO,      KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,
        KC_NO,
        KC_NO,      MO(SWAP),   KC_LSHIFT
    ),
[SWAP] = KEYMAP(
        // left hand
        KC_NO,      KC_NO,      SFT_ENT,    KC_ENT,     KC_NO,      KC_NO,      KC_NO,
        KC_TAB,     SV_Q,       SV_W,       SV_E,       SV_R,       SV_T,       KC_NO,
        KC_ESC,     SV_A,       SV_S,       SV_D,       SV_F,       SV_G,
        KC_NO,      SV_Z,       SV_X,       SV_C,       SV_V,       SV_B,       KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      MO(NAV),
                                                                    KC_NO,      KC_NO,
                                                                                KC_NO,
                                                        KC_SPACE,   MO(SWAP),   KC_NO,
        // right hand
        KC_NO,      KC_NO,      KC_NO,      KC_ENT,     SFT_ENT,    KC_NO,      KC_NO,
        KC_NO,      SV_Y,       SV_U,       SV_I,       SV_O,       SV_P,       MO(FKEYS),
                    SV_H,       SV_J,       SV_K,       SV_L,       MO(NUM),    MO(SYM3),
        KC_NO,      SV_N,       SV_M,       SV_COMM,    SV_DOT,     MO(SYM1),   MO(SYM2),
                    MO(NAV),    KC_NO,      KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,
        KC_NO,
        KC_NO,      MO(SWAP),   KC_LSHIFT
    ),
[NUM] = KEYMAP(
        // left hand
        KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_NO,      KC_NO,
        KC_TAB,     SV_DOT,     SV_7,       SV_8,       SV_9,       SV_MINS,    KC_NO,
        KC_ESC,     SV_0,       SV_4,       SV_5,       SV_6,       SV_PLUS,
        KC_NO,      SV_SLSH,    SV_1,       SV_2,       SV_3,       SV_ASTR,    KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,
                                                                    KC_NO,      KC_NO,
                                                                                KC_NO,
                                                        KC_SPACE,   KC_NO,      KC_NO,
        // right hand
        KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_NO,      KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_TRNS,
                    KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,
                    KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,
        KC_NO,
        KC_NO,      KC_NO,      KC_LSHIFT
    ),
[SYM1] = KEYMAP(
        // left hand
        KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_NO,      KC_NO,
        KC_TAB,     SV_SECT,    SV_LT,      SV_LBRC,    SV_RBRC,    SV_GT,      KC_NO,
        KC_ESC,     SV_EQL,     SV_PIPE,    SV_LPRN,    SV_RPRN,    SV_AMPR,
        KC_NO,      SV_PERC,    CIRC,       SV_LCBR,    SV_RCBR,    TILD,       KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,
                                                                    KC_NO,      KC_NO,
                                                                                KC_NO,
                                                        KC_SPACE,   KC_NO,      KC_NO,
        // right hand
        KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_NO,      KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_TRNS,
                    KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,
                    KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,
        KC_NO,
        KC_NO,      KC_NO,      KC_LSHIFT
    ),
[SYM2] = KEYMAP(
        // left hand
        KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_NO,      KC_NO,
        KC_TAB,     SV_CURR,    SV_PND,     SV_EURO,    SV_DLR,     SV_UNDS,    KC_NO,
        KC_ESC,     SV_AT,      SV_QUES,    SV_APOS,    SV_QUOT,    SV_EXLM,
        KC_NO,      SV_BSLS,    SV_HALF,    GRAV,       SV_MU,      SV_HASH,    KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,
                                                                    KC_NO,      KC_NO,
                                                                                KC_NO,
                                                        KC_SPACE,   KC_NO,      KC_NO,
        // right hand
        KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_NO,      KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_TRNS,
                    KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,
                    KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,
        KC_NO,
        KC_NO,      KC_NO,      KC_LSHIFT
    ),
[SYM3] = KEYMAP(
        // left hand
        KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_NO,      KC_NO,
        KC_TAB,     KC_NO,      LSFT(SV_AA),LSFT(SV_AE),LSFT(SV_OE),KC_NO,      KC_NO,
        KC_ESC,     KC_NO,      SV_AA,      SV_AE,      SV_OE,      KC_NO,
        KC_NO,      KC_NO,      SV_CIRC,    SV_GRAV,    SV_ACUT,    SV_TILD,    KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,
                                                                    KC_NO,      KC_NO,
                                                                                KC_NO,
                                                        KC_SPACE,   KC_NO,      KC_NO,
        // right hand
        KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_NO,      KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_TRNS,
                    KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,
                    KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,
        KC_NO,
        KC_NO,      KC_NO,      KC_LSHIFT
    ),
[MEDIA] = KEYMAP(
        // left hand
        KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_NO,      KC_NO,
        KC_TAB,     SV_Q,       SV_W,       SV_E,       SV_R,       SV_T,       KC_NO,
        ESC_SHIFT,  SV_A,       SV_S,       SV_D,       SV_F,       SV_G,
        DEL_CTRL,   SV_Z,       SV_X,       SV_C,       SV_V,       SV_B,       KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,
                                                                    KC_NO,      KC_NO,
                                                                                KC_NO,
                                                        KC_SPACE,   KC_NO,      KC_NO,
        // right hand
        KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_NO,      KC_NO,
        KC_NO,      SV_Y,       SV_U,       SV_I,       SV_O,       SV_P,       KC_NO,
                    SV_H,       SV_J,       SV_K,       SV_L,       SV_OE,      KC_NO,
        KC_NO,      SV_N,       SV_M,       SV_COMM,    SV_DOT,     SV_MINS,    BS_CTRL,
                    KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,
        KC_NO,
        KC_NO,      KC_NO,      KC_LSHIFT
    ),
[FKEYS] = KEYMAP(
        // left hand
        KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_NO,      KC_NO,
        KC_TAB,     KC_NO,      KC_F7,      KC_F8,      KC_F9,      KC_F12,     KC_NO,
        KC_ESC,     KC_F10,     KC_F4,      KC_F5,      KC_F6,      KC_F11,
        KC_LSFT,    KC_NO,      KC_F1,      KC_F2,      KC_F3,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,
                                                                    KC_NO,      KC_NO,
                                                                                KC_NO,
                                                        KC_SPACE,   KC_NO,      KC_NO,
        // right hand
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_TRNS,
                    KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,
                    KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,
        KC_NO,
        KC_NO,      KC_NO,      KC_LSHIFT
    ),
[NAV] = KEYMAP(
        // left hand
        KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_NO,      KC_NO,
        KC_TAB,     KC_PSCR,    KC_HOME,    KC_PGUP,    KC_PGDOWN,  KC_END,     KC_NO,
        KC_ESC,     KC_INS,     KC_LEFT,    KC_UP,      KC_DOWN,    KC_RIGHT,
        KC_LSHIFT,  OS_LGUI,    OS_LALT,    OS_LCTL,    OS_LSFT,    KC_MENU,    KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      MO(NAV),
                                                                    RESET,      DEBUG,
                                                                                KC_NO,
                                                        KC_SPACE,   KC_NO,      KC_NO,
        // right hand
        KC_NO,      KC_NO,      KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_NO,      KC_NO,
        KC_NO,      KC_HOME,    KC_PGDOWN,  KC_PGUP,    KC_END,     KC_PSCREEN, KC_NO,
                    KC_LEFT,    KC_DOWN,    KC_UP,      KC_RIGHT,   KC_INS,     KC_NO,
        KC_NO,      KC_NO,      OS_RSFT,    OS_RCTL,    OS_LALT,    OS_LGUI,    KC_NO,
                    MO(NAV),    KC_NO,      KC_NO,      KC_NO,      KC_NO,
        DEBUG,      RESET,
        KC_PAUSE,
        KC_NO,      KC_NO,      KC_LSHIFT
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
    // Let QMK handle onshots
    return true;
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
