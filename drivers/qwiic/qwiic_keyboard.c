/* Copyright 2018 Jack Humbert
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

#include "qwiic_keyboard.h"
#include "keymap.h"
#include "matrix.h"
#include "keyboard.h"
#include "twi2c.h"
#include <string.h>
#include "usb_main.h"
#include "usb_driver.h"
#ifdef MULTILAYOUT
#include "multilayout.h"
#endif

#define QWIIC_KEYBOARD_LAYERS 16
#define QWIIC_KEYBOARD_ROWS 8
#define QWIIC_KEYBOARD_COLS 8
#define QWIIC_KEYBOARD_MAX_CONNECTED 8

#define qwiic_matrix_t uint8_t

#define QWIIC_KEYBOARD_HANDSHAKE_ADDRESS       0b01010100
#define QWIIC_KEYBOARD_LISTENING_ADDRESS_START 0b01010110
#define QWIIC_KEYBOARD_HANDSHAKE_MESSAGE_SIZE  (QWIIC_KEYBOARD_LAYERS * QWIIC_KEYBOARD_ROWS * QWIIC_KEYBOARD_COLS)
#define QWIIC_KEYBOARD_MATRIX_MESSAGE_SIZE     MATRIX_ROWS

void qwiic_keyboard_write_keymap(uint8_t * pointer);
void qwiic_keyboard_read_keymap(uint8_t * pointer);

static bool qwiic_keyboard_master = false;
static bool qwiic_keyboard_connected = false;
static uint8_t qwiic_keyboard_handshake_message[QWIIC_KEYBOARD_HANDSHAKE_MESSAGE_SIZE] = {0};
static uint8_t qwiic_keyboard_matrix_message[QWIIC_KEYBOARD_ROWS] = {0};
static qwiic_matrix_t matrix_prev[QWIIC_KEYBOARD_ROWS] = {0};
static twi2c_message_received qwiic_keyboard_message_received_ptr = qwiic_keyboard_message_received;

static uint16_t qwiic_keyboard_keymap[QWIIC_KEYBOARD_LAYERS][QWIIC_KEYBOARD_ROWS][QWIIC_KEYBOARD_COLS] = {{{0}}};
static uint8_t qwiic_keyboard_listening_address = QWIIC_KEYBOARD_LISTENING_ADDRESS_START;

typedef struct {
  unique_id_t hardware_id;
  unique_id_t manual_id;
  bool used_in_multilayout;
} connected_keyboard_t;

// Note: this array include the master as the first entry
static connected_keyboard_t connected_keyboards[QWIIC_KEYBOARD_MAX_CONNECTED + 1];
static uint8_t num_connected_keyboards;

#ifdef MULTILAYOUT
multilayout_status_t multilayout_status;
static bool should_refresh_multilayout = false;

static void clear_multilayout(void) {
  for (uint8_t i = 0; i < NUM_MULTILAYOUT_CONFIGURATIONS; i++) {
    multilayout_module_status_t* status = &multilayout_status.statuses[i];
    status->connected = false;
  }

  for (uint8_t i = 0; i < num_connected_keyboards; i++) {
    connected_keyboards[i].used_in_multilayout = false;
  }
}

static void use_multilayout(connected_keyboard_t* keyboard, multilayout_module_status_t* status) {
  status->connected = true;
  status->keyboard_index = keyboard - connected_keyboards;
  keyboard->used_in_multilayout = true;
}

static void refresh_multilayout(void) {
  clear_multilayout();
  // First pass, assign ids that match, in a first match, first served fashion
  for (uint8_t i = 0; i < NUM_MULTILAYOUT_CONFIGURATIONS; i++) {
    multilayout_module_status_t* status = &multilayout_status.statuses[i];
    multilayout_module_configuration_t* config = &multilayout_configuration.configurations[i];
    if (status->connected == false) {
      for (uint8_t j = 0; j < num_connected_keyboards; j++) {
        connected_keyboard_t* keyboard = &connected_keyboards[j];
        if (keyboard->used_in_multilayout == false) {
          if (are_unique_ids_same(config->id, keyboard->hardware_id) ||
              are_unique_ids_same(config->id, keyboard->manual_id)) {
                use_multilayout(keyboard, status);
          }
        }
      }
    }
  }
  // Then try to assign the rest
  for (uint8_t j = 0; j < num_connected_keyboards; j++) {
    connected_keyboard_t* keyboard = &connected_keyboards[j];
    if (keyboard->used_in_multilayout == false) {
      for (uint8_t i = 0; i < NUM_MULTILAYOUT_CONFIGURATIONS; i++) {
        multilayout_module_status_t* status = &multilayout_status.statuses[i];
        if (status->connected == false) {
          use_multilayout(keyboard, status);
        }
      }
    }
  }
  should_refresh_multilayout = false;
}
#endif

void qwiic_keyboard_init(void) {
  twi2c_init();
  twi2c_start();
  twi2c_start_listening(QWIIC_KEYBOARD_HANDSHAKE_ADDRESS, qwiic_keyboard_message_received_ptr);
}

void qwiic_keyboard_set_master(void) {
  twi2c_stop();
  twi2c_start();
  qwiic_keyboard_master = true;
}

uint8_t command[1] = { 0x00 };

void qwiic_keyboard_task(void) {
  if (USB_DRIVER.state == USB_ACTIVE && qwiic_keyboard_master == false) {
    qwiic_keyboard_master = true;
    num_connected_keyboards = 1;
    assign_unique_id(connected_keyboards[0].hardware_id, get_hardware_unique_id());
    assign_unique_id(connected_keyboards[0].manual_id, get_manual_unique_id());
    connected_keyboards[0].used_in_multilayout = false;
#ifdef MULTILAYOUT
    should_refresh_multilayout = true;
#endif
  }
  else {
    qwiic_keyboard_master = false;
  }
  if (qwiic_keyboard_master) {
    if (qwiic_keyboard_connected) {
      // send empty message, expecting matrix info
      if (MSG_OK != twi2c_transmit_receive(qwiic_keyboard_listening_address,
        command, 1,
        qwiic_keyboard_matrix_message, QWIIC_KEYBOARD_MATRIX_MESSAGE_SIZE
      )) {
        // disconnect
        // qwiic_keyboard_connected = false;
      }
    } else { // if not connected
      // send new address to listen on, expect back keymap
      if (MSG_OK == twi2c_transmit_receive(QWIIC_KEYBOARD_HANDSHAKE_ADDRESS,
        &qwiic_keyboard_listening_address, 1,
        qwiic_keyboard_handshake_message, QWIIC_KEYBOARD_HANDSHAKE_MESSAGE_SIZE
      )) {
        qwiic_keyboard_connected = true;
        // load keymap into memory
        qwiic_keyboard_read_keymap(qwiic_keyboard_handshake_message);
      }
    }
  }
#ifdef MULTILAYOUT
  if (should_refresh_multilayout) {
    refresh_multilayout();
  }
#endif
}

float song_one_up[][2]  = SONG(ONE_UP_SOUND);
bool first_message = true;

void qwiic_keyboard_message_received(I2CDriver *i2cp, uint8_t * body, uint16_t size) {
  if (qwiic_keyboard_connected) {
    for (uint8_t row = 0; row < QWIIC_KEYBOARD_ROWS; row++) {
      if (row < MATRIX_ROWS) {
        qwiic_keyboard_matrix_message[row] = matrix_get_row(row);
      } else {
        qwiic_keyboard_matrix_message[row] = 0;
      }
    }
    twi2c_reply(i2cp, qwiic_keyboard_matrix_message, QWIIC_KEYBOARD_MATRIX_MESSAGE_SIZE);
    if (first_message) {
      PLAY_SONG(song_one_up);
      first_message = false;
    }
  } else {
    qwiic_keyboard_connected = true;
    qwiic_keyboard_master = false;
    qwiic_keyboard_listening_address = body[0];
    twi2c_restart_listening(qwiic_keyboard_listening_address);
    qwiic_keyboard_write_keymap(qwiic_keyboard_handshake_message);
    twi2c_reply(i2cp, qwiic_keyboard_handshake_message, QWIIC_KEYBOARD_HANDSHAKE_MESSAGE_SIZE);
  }
}

// qwiic_keyboard_message_received_ptr = qwiic_keyboard_message_received;

__attribute__((optimize("O0")))
void qwiic_keyboard_write_keymap(uint8_t * pointer) {
#ifdef MULTILAYOUT
#else
  for (uint8_t layer = 0; layer < QWIIC_KEYBOARD_LAYERS; layer++) {
    for (uint8_t row = 0; row < QWIIC_KEYBOARD_ROWS; row++) {
      for (uint8_t col = 0; col < QWIIC_KEYBOARD_COLS; col++) {
        uint16_t keycode = pgm_read_word(&keymaps[layer][row][col]);
        *pointer++ = (keycode >> 8);
        *pointer++ = (keycode & 0xFF);
      }
    }
  }
#endif
}

void qwiic_keyboard_read_keymap(uint8_t * pointer) {
  for (uint8_t layer = 0; layer < QWIIC_KEYBOARD_LAYERS; layer++) {
    for (uint8_t row = 0; row < QWIIC_KEYBOARD_ROWS; row++) {
      for (uint8_t col = 0; col < QWIIC_KEYBOARD_COLS; col++) {
        uint16_t keycode = ((*pointer++) << 8);
        keycode |= (*pointer++);
        qwiic_keyboard_keymap[layer][row][col] = keycode;
      }
    }
  }
}

// overwrite the built-in function - slaves don't need to process keycodes
bool is_keyboard_master(void) {
  return qwiic_keyboard_master;
}

// overwrite the built-in function
uint16_t keymap_key_to_keycode(uint8_t layer, keymatrix_t key) {
#ifdef MULTILAYOUT
  return KC_NO;
#else
  if (key.matrix == 0) {
    // Read entire word (16bits)
    return pgm_read_word(&keymaps[(layer)][(key.pos.row)][(key.pos.col)]);
  } else {
    return qwiic_keyboard_keymap[(layer)][(key.pos.row)][(key.pos.col)];
  }
#endif
}

uint8_t multimatrix_get_num_matrices(void) {
  return qwiic_keyboard_connected ? 1 : 0;
}

uint8_t multimatrix_get_num_cols(uint8_t matrix) {
  return QWIIC_KEYBOARD_COLS;
}

uint8_t multimatrix_get_num_rows(uint8_t matrix) {
  return QWIIC_KEYBOARD_ROWS;
}

uint32_t multimatrix_get_row(uint8_t matrix, uint8_t row) {
return qwiic_keyboard_matrix_message[row];
}

uint32_t multimatrix_get_row_cache(uint8_t matrix, uint8_t row) {
  return matrix_prev[row];
}

void multimatrix_set_row_cache(uint8_t matrix, uint8_t row, uint32_t value) {
  matrix_prev[row] = value;
}

uint8_t* multimatrix_get_source_layers_cache(uint8_t matrix) {
    static uint8_t source_layers_cache[(QWIIC_KEYBOARD_ROWS * QWIIC_KEYBOARD_COLS * MAX_LAYER_BITS + 7) / 8] = {0};
    return source_layers_cache;
}
