#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "hal.h"
#include "timer.h"
#include "wait.h"
#include "print.h"
#include "backlight.h"
#include "matrix.h"
#include "action.h"
#include "keycode.h"
#include <string.h>
#include "quantum.h"

#if (DIODE_DIRECTION != ROW2COL) && (DIODE_DIRECTION != COL2ROW)
#error "DIODE_DIRECTION not defined"
#endif

/*
 *     col: { A10, B2, A15, A0, A1, A2, B0, B1, C13, A6, A7, A3 }
 *     row: { B5, B10, A9, A8 }
 */
/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];
static bool debouncing = false;
static uint16_t debouncing_time = 0;

static LINE_TYPE matrix_col_pins[MATRIX_COLS] = MATRIX_COL_PINS;
static LINE_TYPE matrix_row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS;

__attribute__ ((weak))
void matrix_init_user(void) {}

__attribute__ ((weak))
void matrix_scan_user(void) {}

__attribute__ ((weak))
void matrix_init_kb(void) {
  matrix_init_user();
}

__attribute__ ((weak))
void matrix_scan_kb(void) {
  matrix_scan_user();
}

void matrix_init(void) {
    xprintf("matrix init\n");
    //debug_matrix = true;

    // actual matrix setup
    for (int i = 0; i < MATRIX_ROWS; i++) {
      setPadMode(matrix_row_pins[i], PAL_MODE_OUTPUT_PUSHPULL);
      #if DIODE_DIRECTION == ROW2COL
        clearPad(matrix_row_pins[i]);
      #else
        setPad(matrix_row_pins[i]);
      #endif
    }

    for (int i = 0; i < MATRIX_COLS; i++) {
      setPadMode(matrix_col_pins[i], DIODE_DIRECTION == ROW2COL ? PAL_MODE_INPUT_PULLDOWN  : PAL_MODE_INPUT_PULLUP);
    }

    memset(matrix, 0, MATRIX_ROWS * sizeof(matrix_row_t));
    memset(matrix_debouncing, 0, MATRIX_COLS * sizeof(matrix_row_t));

    matrix_init_quantum();
}

uint8_t matrix_scan(void) {


    // actual matrix
    for (int row = 0; row < MATRIX_ROWS; row++) {
        matrix_row_t data = 0;

        #if DIODE_DIRECTION == ROW2COL
          setPad(matrix_row_pins[row]);
        #else
          clearPad(matrix_row_pins[row]);
        #endif

        // need wait to settle pin state
        wait_us(20);

        for (int col = 0; col < MATRIX_COLS; col++) {
          data |= (readPad(matrix_col_pins[col]) << col);
        }

        #if DIODE_DIRECTION == ROW2COL
          clearPad(matrix_row_pins[row]);
        #else
          setPad(matrix_row_pins[row]);
          data = ~data;
        #endif

        if (matrix_debouncing[row] != data) {
            matrix_debouncing[row] = data;
            debouncing = true;
            debouncing_time = timer_read();
        }
    }

    if (debouncing && timer_elapsed(debouncing_time) > DEBOUNCE) {
        memcpy(matrix, matrix_debouncing, sizeof(matrix));
        debouncing = false;
    }

    matrix_scan_quantum();

    return 1;
}

bool matrix_is_on(uint8_t row, uint8_t col) {
    return (matrix[row] & (1<<col));
}

matrix_row_t matrix_get_row(uint8_t row) {
    return matrix[row];
}

void matrix_print(void) {
    xprintf("\nr/c 01234567\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        xprintf("%X0: ", row);
        matrix_row_t data = matrix_get_row(row);
        for (int col = 0; col < MATRIX_COLS; col++) {
            if (data & (1<<col))
                xprintf("1");
            else
                xprintf("0");
        }
        xprintf("\n");
    }
}
