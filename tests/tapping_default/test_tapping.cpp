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

#include "test_common.hpp"
#include "action_tapping.h"

using testing::_;
using testing::InSequence;

class Tapping : public TestFixture {};

void press_a() {
  press_key(0, 0);
}

void release_a() {
  release_key(0, 0);
}

void press_shift_t_b() {
  press_key(1, 0);
}

void release_shift_t_b() {
  release_key(1, 0);
}

TEST_F(Tapping, TapSFT_T) {
    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    // Tapping keys does nothing on press
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
    release_shift_t_b();
    // First we get the key press
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
    // Then the release
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    run_one_scan_loop();
}

TEST_F(Tapping, HoldSFT_T) {
    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    // Tapping keys does nothing on press
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM);
    testing::Mock::VerifyAndClearExpectations(&driver);
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    run_one_scan_loop();
}

TEST_F(Tapping, TapSFT_TTwiceInARow) {

    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    // Tapping keys does nothing on press
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
    release_shift_t_b();
    // First we get the key press
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
    // Then the release
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    run_one_scan_loop();

    press_shift_t_b();
    #ifdef TAPPING_FORCE_HOLD
      // Tapping keys does nothing on press
      EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
      run_one_scan_loop();
      testing::Mock::VerifyAndClearExpectations(&driver);

      release_shift_t_b();
      // The tap key is sent on release
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
      // Then the release
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #else
      // When TAPPING_FORCE_HOLD is not enabled we get the key press immediately
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
      run_one_scan_loop();
      testing::Mock::VerifyAndClearExpectations(&driver);
      release_shift_t_b();
      // And release too
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
}

TEST_F(Tapping, TapShiftTThenHold) {
    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    // Tapping keys does nothing on press
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
    release_shift_t_b();
    // When the key is relased we get the tap
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
    // Followed by the release
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    run_one_scan_loop();

    press_shift_t_b();
    #ifdef TAPPING_FORCE_HOLD
      // The modifier will not be pressed until the tapping term
      EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
      idle_for(TAPPING_TERM - 1);
      testing::Mock::VerifyAndClearExpectations(&driver);
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      run_one_scan_loop();
    #else
      // Without force hold, the tap key is sent immediately
      // NOTE: that the tap key, not the modifier is sent
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
      run_one_scan_loop();
    #endif

    testing::Mock::VerifyAndClearExpectations(&driver);
    run_one_scan_loop();
    release_shift_t_b();
    // The release is sent when the key is released
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #if defined(TAPPING_FORCE_HOLD) && defined(RETRO_TAPPING)
      // BUG:
      // When both TAPPING_FORCE_HOLD and RETRO_TAPPING are enabled, we get an extra tap key
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
}

TEST_F(Tapping, TapOtherKeyWhenHoldingSFT_T) {
    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    // Tapping keys does nothing on press
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
    press_a();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
    release_a();
    #ifdef (PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #elif defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #endif

    release_shift_t_b();
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if !defined(PERMISSIVE_HOLD) && !defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    idle_for(TAPPING_TERM-3);
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
}

#if 0
TEST_F(Tapping, ANewTapWithinTappingTermIsBuggy) {
    // See issue #1478 for more information
    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    // Tapping keys does nothing on press
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    release_shift_t_b();
    // First we get the key press
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
    // Then the release
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    run_one_scan_loop();

    // This sends KC_B, even if it should do nothing
    press_shift_t_b();
    // This test should not succed if everything works correctly
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
    run_one_scan_loop();
    release_shift_t_b();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    idle_for(TAPPING_TERM + 1);

    // On the other hand, nothing is sent if we are outside the tapping term
    press_shift_t_b();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    release_shift_t_b();

    // First we get the key press
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
    // Then the release
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    idle_for(TAPPING_TERM + 1);

    // Now we are geting into strange territory, as the hold registers too early here
    // But the stranges part is:
    // If TAPPING_TERM + 1 above is changed to TAPPING_TERM or TAPPING_TERM + 2 it doesn't
    press_shift_t_b();
    // Shouldn't be called here really
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT))).Times(1);
    idle_for(TAPPING_TERM);
}
#endif
