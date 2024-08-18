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
 
#include "action.h"
#include "action_layer.h"
#include "quantum_keycodes.h"
#include "keymap.h"

static bool is_layer_switch(uint16_t keycode) {
    // Tapping is not supported
    switch(keycode) {
    case QK_TO...QK_TO_MAX:
    case QK_MOMENTARY...QK_MOMENTARY_MAX:
    case QK_TOGGLE_LAYER...QK_TOGGLE_LAYER_MAX:
    case QK_ONE_SHOT_LAYER...QK_ONE_SHOT_LAYER_MAX:
        return true;
    }
    return false;
}
 
void process_lenient_layer(keyrecord_t* record) {
    keypos_t keypos=record->event.key;
    uint16_t keycode=keymap_key_to_keycode(layer_switch_get_layer(keypos), keypos);
    if (is_layer_switch(keycode)) {
        
    }
    else {
        record->event.key.row=255;
        record->event.key.col=255;
    }
}
