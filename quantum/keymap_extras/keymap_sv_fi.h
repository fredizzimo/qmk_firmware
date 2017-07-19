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

// Swedish and Finnish keymaps (they are the same), includes Sami characters

 #pragma once

 #include "keycode.h"

// Alt Gr
#define ALGR(kc) RALT(kc)
#define SV_ALGR KC_RALT

// Normal characters
#define SV_SECT KC_GRV  // §
#define SV_1 KC_1       // 1
#define SV_2 KC_2       // 2
#define SV_3 KC_3       // 3
#define SV_4 KC_4       // 4
#define SV_5 KC_5       // 5
#define SV_6 KC_6       // 6
#define SV_7 KC_7       // 7
#define SV_8 KC_8       // 8
#define SV_9 KC_9       // 9
#define SV_0 KC_0       // 0
#define SV_PLUS KC_MINS // +
#define SV_ACUT KC_EQL  // ´ (dead character)

#define SV_Q KC_Q       // q
#define SV_W KC_W       // w
#define SV_E KC_E       // e
#define SV_R KC_R       // r
#define SV_T KC_T       // t
#define SV_Y KC_Y       // y
#define SV_U KC_U       // u
#define SV_I KC_I       // i
#define SV_O KC_O       // o
#define SV_P KC_P       // p
#define SV_AA KC_LBRC   // å
#define SV_UMLT KC_RBRC // ¨ (dead character)

#define SV_A KC_A       // a
#define SV_S KC_S       // s
#define SV_D KC_D       // d
#define SV_F KC_F       // f
#define SV_G KC_G       // g
#define SV_H KC_H       // h
#define SV_J KC_J       // j
#define SV_K KC_K       // k
#define SV_L KC_L       // l
#define SV_OE KC_SCLN   // ö
#define SV_AE KC_LBRC   // ä
#define SV_APOS KC_NUHS // '

#define SV_LT KC_LT     // <
#define SV_Z KC_Z       // z
#define SV_X KC_X       // x
#define SV_C KC_C       // c
#define SV_V KC_V       // v
#define SV_B KC_B       // b
#define SV_N KC_N       // n
#define SV_M KC_M       // m
#define SV_COMM KC_COMMA    // ,
#define SV_DOT KC_DOT   // .
#define SV_MINS KC_SLSH // -

// Shifted characters
#define SV_HALF LSFT(SV_SECT)   // ½
#define SV_EXLM LSFT(SV_1)      // !
#define SV_QUOT LSFT(SV_2)      // "
#define SV_HASH LSFT(SV_3)      // #
#define SV_CURR LSFT(SV_4)      // ¤
#define SV_PERC LSFT(SV_5)      // %
#define SV_AMPR LSFT(SV_6)      // &
#define SV_SLSH LSFT(SV_7)      // /
#define SV_LPRN LSFT(SV_8)      // (
#define SV_RPRN LSFT(SV_9)      // )
#define SV_EQL LSFT(SV_0)       // =
#define SV_QUES LSFT(SV_PLUS)   // ?
#define SV_GRAV LSFT(SV_ACUT)   // ` (dead character)

#define SV_CIRC LSFT(SV_UMLT)   // ^ (dead character)

#define SV_ASTR LSFT(SV_APOS)   // *

#define SV_GT LSFT(SV_LT)       // >
#define SV_SCLN LSFT(SV_COMM)   // ;
#define SV_COLN LSFT(SV_DOT)    // :
#define SV_UNDS LSFT(SV_MINS)   // _

// Alt Gr characters
#define SV_AT ALGR(SV_2)        // @
#define SV_PND ALGR(SV_3)       // £
#define SV_DLR ALGR(SV_4)       // $
#define SV_LCBR ALGR(SV_7)      // {
#define SV_LBRC ALGR(SV_8)      // [
#define SV_RBRC ALGR(SV_9)      // ]
#define SV_RCBR ALGR(SV_0)      // }
#define SV_BSLS ALGR(SV_PLUS) // \ no multiline comment

#define SV_ACIRC ALGR(SV_Q)     // â
#define SV_EURO ALGR(SV_E)       // €
#define SV_TSTR ALGR(SV_T)      // ŧ
#define SV_IUML ALGR(SV_I)      // ï
#define SV_OTILD ALGR(SV_O)     // õ
#define SV_TILD ALGR(SV_UMLT)   // ~ (dead character)

#define SV_AACUT ALGR(SV_A)     // á
#define SV_SCAR ALGR(SV_S)      // š
#define SV_DSTR ALGR(SV_D)      // đ
#define SV_GSTR ALGR(SV_F)      // ǥ
#define SV_GCAR ALGR(SV_G)      // ǧ
#define SV_HCAR ALGR(SV_H)      // ȟ
#define SV_KCAR ALGR(SV_K)      // ǩ
#define SV_OSTR ALGR(SV_OE)     // ø
#define SV_ASH ALGR(SV_AE)      // æ

#define SV_PIPE ALGR(SV_LT)     // |
#define SV_ZCAR ALGR(SV_Z)      // ž
#define SV_CCAR ALGR(SV_C)      // č
#define SV_EZHCAR ALGR(SV_V)    // ǯ
#define SV_EZH ALGR(SV_B)       // ʒ
#define SV_ENG ALGR(SV_N)       // ŋ
#define SV_MU ALGR(SV_M)        // µ