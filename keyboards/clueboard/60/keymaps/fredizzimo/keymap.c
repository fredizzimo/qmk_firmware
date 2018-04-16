#include "60.h"
#include "keymap_sv_fi.h"

#define _______ KC_TRNS

enum keyboard_layers {
    BASE,
    FUNC,
    EXTRA,
};

enum custom_keycodes {
    S_BSKTC = SAFE_RANGE,
    S_ODEJY,
    S_RCKBY,
    S_DOEDR,
    S_SCALE,
    S_ONEUP,
    S_COIN,
    S_SONIC,
    S_ZELDA
};

#ifdef AUDIO_ENABLE
  float song_basketcase[][2] = SONG(BASKET_CASE);
  float song_ode_to_joy[][2]  = SONG(ODE_TO_JOY);
  float song_rock_a_bye_baby[][2]  = SONG(ROCK_A_BYE_BABY);
  float song_doe_a_deer[][2]  = SONG(DOE_A_DEER);
  float song_scale[][2]  = SONG(MUSIC_SCALE_SOUND);
  float song_coin[][2]  = SONG(COIN_SOUND);
  float song_one_up[][2]  = SONG(ONE_UP_SOUND);
  float song_sonic_ring[][2]  = SONG(SONIC_RING);
  float song_zelda_puzzle[][2]  = SONG(ZELDA_PUZZLE);
#endif

#define MO_FUNC MO(FUNC)
#define MO_EXTR MO(EXTRA)
#define SP_FUNC LT(FUNC, KC_SPC)
#define VS_CMD OSM(MOD_LSFT | MOD_LCTL)

const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = KEYMAP(
      KC_MPLY, SV_1,    SV_2,    SV_3,    SV_4,    SV_5,    SV_6,    SV_7,    SV_8,   SV_9,    SV_0,    SV_PLUS, SV_ACUT, KC_BSPC, KC_DEL,\
      KC_TAB,  SV_Q,    SV_W,    SV_E,    SV_R,    SV_T,    SV_Y,    SV_U,    SV_I,   SV_O,    SV_P,    VS_CMD,   SV_UMLT, KC_NO,  \
      KC_ESC,  SV_A,    SV_S,    SV_D,    SV_F,    SV_G,    SV_H,    SV_J,    SV_K,   SV_L,    SV_OE,   SV_AE,   SV_APOS, KC_ENT, \
      KC_LSFT, SV_LT,   SV_Z,    SV_X,    SV_C,    SV_V,    SV_B,    SV_N,    SV_M,   SV_COMM, SV_DOT,  SV_MINS, KC_RSFT, VS_CMD, \
      KC_LCTL, KC_LGUI, KC_LALT,                        SP_FUNC,                      KC_RALT, KC_APP,  MO_FUNC, KC_RCTL),
    [FUNC] = KEYMAP(
      KC_ESC,  KC_F1,  KC_F2,    KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,  KC_F9,    KC_F10,  KC_F11,  KC_F12,  _______, RESET,\
      _______, KC_INS,  KC_HOME, KC_UP,   KC_END,  KC_PGUP, _______, _______, _______, _______, _______, _______, _______, _______, \
      _______, MO_EXTR, KC_LEFT, KC_DOWN, KC_RGHT, KC_PGDN, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, _______, _______, _______, \
      _______, KC_PSCR, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
      _______, _______, _______,                        _______,                       _______, _______, _______, _______),
    [EXTRA] = KEYMAP( BL_STEP, S_BSKTC, S_ODEJY, S_RCKBY, S_DOEDR, S_SCALE, S_ONEUP, S_COIN, S_SONIC, S_ZELDA,  _______, _______, _______, _______, _______,\
      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
      _______, _______, _______,                        _______,                       _______, _______, _______, _______)
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case S_BSKTC:
            if (record->event.pressed) {
                stop_all_notes();
                PLAY_SONG(song_basketcase);
            }
            return false;
        case S_ODEJY:
            if (record->event.pressed) {
                stop_all_notes();
                PLAY_SONG(song_ode_to_joy);
            }
            return false;
        case S_RCKBY:
            if (record->event.pressed) {
                stop_all_notes();
                PLAY_SONG(song_rock_a_bye_baby);
            }
            return false;
        case S_DOEDR:
            if (record->event.pressed) {
                stop_all_notes();
                PLAY_SONG(song_doe_a_deer);
            }
            return false;
        case S_SCALE:
            if (record->event.pressed) {
                stop_all_notes();
                PLAY_SONG(song_scale);
            }
            return false;
        case S_ONEUP:
            if (record->event.pressed) {
                stop_all_notes();
                PLAY_SONG(song_one_up);
            }
            return false;
        case S_COIN:
            if (record->event.pressed) {
                stop_all_notes();
                PLAY_SONG(song_coin);
            }
            return false;
        case S_SONIC:
            if (record->event.pressed) {
                stop_all_notes();
                PLAY_SONG(song_sonic_ring);
            }
            return false;
        case S_ZELDA:
            if (record->event.pressed) {
                stop_all_notes();
                PLAY_SONG(song_zelda_puzzle);
            }
            return false;
    }
    return true;
}
