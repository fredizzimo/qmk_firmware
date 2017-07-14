/* Copyright 2017 Fred Sundvik
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#define KEYMAP( \
    A80, A70, A60, A50, A40, A30, A20,  \
    A81, A71, A61, A51, A41, A31, A21,  \
    A82, A72, A62, A52, A42, A32,       \
    A83, A73, A63, A53, A43, A33, A23,  \
    A84, A74, A64, A54, A44,            \
                                  A13, A03, \
                                       A04, \
                             A34, A24, A14, \
         B20, B30, B40, B50, B60, B70, B80, \
         B21, B31, B41, B51, B61, B71, B81, \
              B32, B42, B52, B62, B72, B82, \
         B23, B33, B43, B53, B63, B73, B83, \
                   B44, B54, B64, B74, B84, \
    B03, B13,     \
    B04,          \
    B14, B24, B34 \
) { \
    {A80, A70, A60, A50, A40, A30, A20, B20, B30, B40, B50, B60, B70, B80}, \
    {A81,    A71,  A61,   A51,   A41,   A31,   A21,   B21,   B31,   B41,   B51,   B61,   B71,   B81}, \
    {A82,    A72,  A62,   A52,   A42,   A32,   KC_NO, KC_NO, B32,   B42,   B52,   B62,   B72,   B82}, \
    {A83,    A73,  A63,   A53,   A43,   A33,   A23,   B23,   B33,   B43,   B53,   B63,   B73,   B83}, \
    {A84,    A74,  A64,   A54,   A44,   KC_NO, KC_NO, KC_NO, KC_NO, B44,   B54,   B64,   B74,   B84}, \
    {KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, A13,   A03,   B03,   B13,   KC_NO, KC_NO, KC_NO, KC_NO, KC_NO}, \
    {KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, A04,   B04,   KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO}, \
    {KC_NO, KC_NO, KC_NO, KC_NO, A34,   A24,   A14,   B14,   B24,   B34,   KC_NO, KC_NO, KC_NO, KC_NO}, \
}

inline void ergodox_board_led_on(void) {}
inline void ergodox_right_led_1_on(void) {}
inline void ergodox_right_led_2_on(void) {}
inline void ergodox_right_led_3_on(void) {}
inline void ergodox_right_led_on(uint8_t led) {}
inline void ergodox_board_led_off(void) {}
inline void ergodox_right_led_1_off(void) {}
inline void ergodox_right_led_2_off(void) {}
inline void ergodox_right_led_3_off(void) {}
inline void ergodox_right_led_off(uint8_t led) {}