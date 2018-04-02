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

namespace
{

  class LayerTapping : public TestFixture {};

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

  void press_c() {
    press_key(2, 0);
  }

  void release_c() {
    release_key(2, 0);
  }

  void press_d() {
    press_key(3, 0);
  }

  void release_d() {
    release_key(3, 0);
  }

  void press_ctl() {
    press_key(4, 0);
  }

  void release_ctl() {
    release_key(4, 0);
  }

  void press_ctl_t_e() {
    press_key(5, 0);
  }

  void release_ctl_t_e() {
    release_key(5, 0);
  }

  void press_lt_1() {
    press_key(6, 0);
  }

  void release_lt_1() {
    release_key(6, 0);
  }
}

TEST_F(LayerTapping, TapLT) {
    TestDriver driver;
    InSequence s;

    press_lt_1();
    // Tapping keys does nothing on press
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
    release_lt_1();
    // First we get the key press
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_F)));
    // Then the release
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    run_one_scan_loop();
}

TEST_F(LayerTapping, HoldLT) {
    TestDriver driver;
    InSequence s;

    press_lt_1();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM);
    testing::Mock::VerifyAndClearExpectations(&driver);

    // This extra report is really not needed
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
}

TEST_F(LayerTapping, TapLTTwiceInARow) {

    TestDriver driver;
    InSequence s;

    press_lt_1();
    // Tapping keys does nothing on press
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
    release_lt_1();
    // First we get the key press
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_F)));
    // Then the release
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    run_one_scan_loop();

    press_lt_1();
    #ifdef TAPPING_FORCE_HOLD
      // Tapping keys does nothing on press
      EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
      run_one_scan_loop();
      testing::Mock::VerifyAndClearExpectations(&driver);

      release_lt_1();
      // The tap key is sent on release
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_F)));
      // Then the release
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #else
      // When TAPPING_FORCE_HOLD is not enabled we get the key press immediately
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport((KC_F))));
      run_one_scan_loop();
      testing::Mock::VerifyAndClearExpectations(&driver);
      release_lt_1();
      // And release too
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
}

TEST_F(LayerTapping, TapLTThenHold) {
    TestDriver driver;
    InSequence s;

    press_lt_1();
    // Tapping keys does nothing on press
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
    release_lt_1();
    // When the key is relased we get the tap
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_F)));
    // Followed by the release
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    run_one_scan_loop();

    press_lt_1();
    #ifdef TAPPING_FORCE_HOLD
      // The modifier will not be pressed until the tapping term
      EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
      idle_for(TAPPING_TERM);
      testing::Mock::VerifyAndClearExpectations(&driver);

      // This extra report is not needed
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
      run_one_scan_loop();
    #else
      // Without force hold, the tap key is sent immediately
      // NOTE: that the tap key, not the modifier is sent
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_F)));
      run_one_scan_loop();
    #endif

    testing::Mock::VerifyAndClearExpectations(&driver);
    run_one_scan_loop();
    release_lt_1();
    // The release is sent when the key is released
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #if defined(TAPPING_FORCE_HOLD) && defined(RETRO_TAPPING)
      // BUG:
      // When both TAPPING_FORCE_HOLD and RETRO_TAPPING are enabled, we get an extra tap key
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_F)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM);
    testing::Mock::VerifyAndClearExpectations(&driver);
}

TEST_F(LayerTapping, TapOtherKeyWhenTappingSFT_T) {
    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_a();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_a();
    #ifdef PERMISSIVE_HOLD
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

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 4);
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if !defined(PERMISSIVE_HOLD) && !defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
}

TEST_F(LayerTapping, ReleaseSFT_TBeforeOtherKeyTapBoth) {
    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    run_one_scan_loop();
    press_a();
    run_one_scan_loop();

    release_shift_t_b();
    #ifdef IGNORE_MOD_TAP_INTERRUPT
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A, KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A)));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #elif defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    release_a();
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 4);
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if !defined(IGNORE_MOD_TAP_INTERRUPT) && !defined(PERMISSIVE_HOLD)
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A)));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
}

TEST_F(LayerTapping, HoldSFT_TWhileTappingOtherKeyBeforeTappingTerm) {

    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    run_one_scan_loop();
    press_a();
    run_one_scan_loop();

    release_a();

    #ifdef PERMISSIVE_HOLD
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 3);
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if !defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    release_shift_t_b();
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
}

TEST_F(LayerTapping, HoldSFT_TAndTapOtherKeyAfterTappingTerm) {

    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    run_one_scan_loop();

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 1);
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_a();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
    run_one_scan_loop();

    release_a();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    run_one_scan_loop();

    release_shift_t_b();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    run_one_scan_loop();
}

TEST_F(LayerTapping, HoldSFT_TAndTapOtherKeyAfterTappingTermButReleaseSFT_TFirst) {

    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    run_one_scan_loop();

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 1);
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_a();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
    run_one_scan_loop();

    release_shift_t_b();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A)));
    run_one_scan_loop();

    release_a();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    run_one_scan_loop();
}

TEST_F(LayerTapping, HoldSFT_TAndTapOtherKeyBeforeTappingTermButReleaseAfter) {

    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    run_one_scan_loop();

    press_a();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 2);
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_a();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    run_one_scan_loop();

    release_shift_t_b();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    run_one_scan_loop();
}

TEST_F(LayerTapping, PressKeyBeforeHoldingSFT_T) {

    TestDriver driver;
    InSequence s;

    press_a();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A)));
    run_one_scan_loop();

    press_shift_t_b();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_a();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    run_one_scan_loop();

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 2);
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    run_one_scan_loop();

    release_shift_t_b();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    run_one_scan_loop();
}

TEST_F(LayerTapping, PressKeyBeforeHoldingSFT_TReleaseAfterTappingTerm) {

    TestDriver driver;
    InSequence s;

    press_a();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A)));
    run_one_scan_loop();

    press_shift_t_b();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 2);
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
    run_one_scan_loop();

    release_a();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    run_one_scan_loop();

    release_shift_t_b();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    run_one_scan_loop();
}

TEST_F(LayerTapping, TapTwoOtherKeysWhenTappingSFT_TReleaseSecondBeforeSFT_T) {
    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    run_one_scan_loop();

    press_a();
    run_one_scan_loop();

    release_a();
    #ifdef PERMISSIVE_HOLD
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_c();
    #ifdef PERMISSIVE_HOLD
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_C)));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_c();
    #ifdef PERMISSIVE_HOLD
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #elif defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #endif
    release_shift_t_b();
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 6);
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if !defined(PERMISSIVE_HOLD) && !defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
}

TEST_F(LayerTapping, TapTwoOtherKeysWhenTappingSFT_TReleaseSecondAfterSFT_T) {
    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    run_one_scan_loop();

    press_a();
    run_one_scan_loop();

    release_a();
    #ifdef PERMISSIVE_HOLD
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_c();
    #ifdef PERMISSIVE_HOLD
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_C)));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C)));
    #elif defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C)));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #endif
    release_shift_t_b();
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_c();
    #if defined(IGNORE_MOD_TAP_INTERRUPT) || defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 6);
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if !defined(PERMISSIVE_HOLD) && !defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
}

TEST_F(LayerTapping, TapTwoOtherKeysWhenTappingSFT_TReleaseBothAfterSFT_T) {
    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    run_one_scan_loop();

    press_a();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_c();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B, KC_A, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A, KC_C)));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #endif
    release_shift_t_b();
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_a();
    #if defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C)));
    #elif defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);


    release_c();
    #if defined(IGNORE_MOD_TAP_INTERRUPT) || defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 6);
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if !defined(PERMISSIVE_HOLD) && !defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
}

TEST_F(LayerTapping, TapTwoOtherKeysWhenTappingSFT_TReleaseFirstButNotSecondAfterSFT_T) {
    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    run_one_scan_loop();

    press_a();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_c();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_c();
    #ifdef PERMISSIVE_HOLD
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A)));
    #elif defined(IGNORE_MOD_TAP_INTERRUPT)
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B, KC_A)));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B, KC_A, KC_C)));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B, KC_A)));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A)));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #endif
    release_shift_t_b();
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_a();
    #if defined(PERMISSIVE_HOLD) || defined(IGNORE_MOD_TAP_INTERRUPT)
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 6);
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if !defined(PERMISSIVE_HOLD) && !defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
}

TEST_F(LayerTapping, AComplexCaseWithAFourthKey) {
    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    run_one_scan_loop();

    press_a();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_c();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B, KC_A, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A, KC_C)));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #endif
    release_shift_t_b();
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_a();
    #if defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C)));
    #elif defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_d();
    #if defined(IGNORE_MOD_TAP_INTERRUPT) || defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C, KC_D)));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_c();
    #if defined(IGNORE_MOD_TAP_INTERRUPT) || defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_D)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_d();
    #if defined(IGNORE_MOD_TAP_INTERRUPT) || defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 8);
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if !defined(PERMISSIVE_HOLD) && !defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C, KC_D)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_D)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
}


TEST_F(LayerTapping, TapCtlAndOtherKeysWhenTappingSFT_TReleaseBothAfterSFT_T) {
    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    run_one_scan_loop();

    press_ctl();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_a();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B, KC_LCTL)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B, KC_LCTL, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_A)));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #endif
    release_shift_t_b();
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_ctl();
    #if defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A)));
    #elif defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_LCTL)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_LCTL, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_a();
    #if defined(IGNORE_MOD_TAP_INTERRUPT) || defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 6);
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if !defined(PERMISSIVE_HOLD) && !defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_LCTL)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_LCTL, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
}

TEST_F(LayerTapping, AKeyPressedBeforeTappingIsReleasedAtTheRightTime) {
    TestDriver driver;
    InSequence s;

    press_d();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_D)));
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_shift_t_b();
    run_one_scan_loop();

    press_a();
    run_one_scan_loop();

    release_a();
    #ifdef PERMISSIVE_HOLD
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_D, KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_D, KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_D, KC_LSFT)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_c();
    #ifdef PERMISSIVE_HOLD
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_D, KC_LSFT, KC_C)));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_D, KC_C)));
    #elif defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_D, KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_D, KC_B, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_D, KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_D, KC_B, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_D, KC_C)));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_D, KC_LSFT)));
    #endif
    release_shift_t_b();
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_d();
    #if defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C)));
    #elif defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C)));
    #else
      // Process release event of a key pressed before tapping starts
      // Without this unexpected repeating will occur with having fast repeating setting
      // https://github.com/tmk/tmk_keyboard/issues/60
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_c();
    #if defined(IGNORE_MOD_TAP_INTERRUPT) || defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 7);
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if !defined(PERMISSIVE_HOLD) && !defined(IGNORE_MOD_TAP_INTERRUPT)
      //NOTE: KC_D was already released before, see the comment above
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
}

TEST_F(LayerTapping, CtrlPressedBeforeTappingIsReleasedAtTheRightTime) {
    TestDriver driver;
    InSequence s;

    press_ctl();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL)));
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_shift_t_b();
    run_one_scan_loop();

    press_a();
    run_one_scan_loop();

    release_a();
    #ifdef PERMISSIVE_HOLD
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_LSFT)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_c();
    #ifdef PERMISSIVE_HOLD
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_LSFT, KC_C)));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_C)));
    #elif defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_B, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_B, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_C)));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_LSFT)));
    #endif
    release_shift_t_b();
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_ctl();
    #if defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C)));
    #elif defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_c();
    #if defined(IGNORE_MOD_TAP_INTERRUPT) || defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 7);
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if !defined(PERMISSIVE_HOLD) && !defined(IGNORE_MOD_TAP_INTERRUPT)
      //BUG: This call is redundant, it was already called before
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_LSFT, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
}

TEST_F(LayerTapping, CtrlPressedBeforeTappingAndReleasedBeforeSecondKey) {
    TestDriver driver;
    InSequence s;

    press_ctl();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL)));
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_shift_t_b();
    run_one_scan_loop();

    press_a();
    run_one_scan_loop();

    release_a();
    #ifdef PERMISSIVE_HOLD
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_LSFT)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_ctl();
    #if defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_c();
    #if defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_C)));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C)));
    #elif defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_B, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C)));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_LSFT)));
    #endif
    release_shift_t_b();
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_c();
    #if defined(IGNORE_MOD_TAP_INTERRUPT) || defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 7);
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if !defined(PERMISSIVE_HOLD) && !defined(IGNORE_MOD_TAP_INTERRUPT)
      //BUG: This call is redundant, it was already called before
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_LSFT, KC_A)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_C)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
}

TEST_F(LayerTapping, TapShiftTThenHoldAndType) {
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
    #else
      // Without force hold, the tap key is sent immediately
      // NOTE: that the tap key, not the modifier is sent
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_a();
    #if defined(TAPPING_FORCE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    #else
      // The next key is also sent immediately without force hold
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B, KC_A)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 2);
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if defined(TAPPING_FORCE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_A)));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_shift_t_b();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A)));
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_a();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
}

TEST_F(LayerTapping, TapSecondTapKeyWhileHoldingDownTheFirstOne) {
    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_ctl_t_e();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_shift_t_b();
    #if defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_ctl_t_e();
    #if defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_E)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #elif defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_E)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_E)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 4);
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if !defined(IGNORE_MOD_TAP_INTERRUPT) && !defined(PERMISSIVE_HOLD)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_E)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_E)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
}

TEST_F(LayerTapping, TapFirstTapKeyAndHoldSecondOne) {
    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_ctl_t_e();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_shift_t_b();
    #if defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 3);
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    #else
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL)));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_LCTL)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_ctl_t_e();
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
}

TEST_F(LayerTapping, TapFirstHoldTapSecondWhileTappingKey) {
    TestDriver driver;
    InSequence s;

    press_shift_t_b();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_ctl_t_e();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_shift_t_b();
    #if defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_B)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    press_a();
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_a();
    #if defined(PERMISSIVE_HOLD)
      #if defined(IGNORE_MOD_TAP_INTERRUPT)
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL)));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL, KC_A)));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LCTL)));
      #else
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
      #endif
    #else
      EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    release_ctl_t_e();
    #if defined(PERMISSIVE_HOLD)
      #if defined(IGNORE_MOD_TAP_INTERRUPT)
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
      #else
        //BUG: Permissive hold only applies to the first tap key
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_E)));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_E)));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A, KC_E)));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_E)));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
      #endif
    #elif defined(IGNORE_MOD_TAP_INTERRUPT)
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_E)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A, KC_E)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_E)));
      EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #else
      EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);

    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    idle_for(TAPPING_TERM - 6);
    testing::Mock::VerifyAndClearExpectations(&driver);

    #if !defined(PERMISSIVE_HOLD) && !defined(IGNORE_MOD_TAP_INTERRUPT)
        //BUG: Permissive hold only applies to the first tap key
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT)));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_LSFT, KC_E)));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_E)));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A, KC_E)));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_E)));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport()));
    #endif
    run_one_scan_loop();
    testing::Mock::VerifyAndClearExpectations(&driver);
}
