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
using testing::_;
using testing::AnyNumber;
using testing::Mock;

class LLSwitch : public TestFixture {};

TEST_F(LLSwitch, ANormalKeyIsDelayed) {
    TestDriver driver;
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(0);
    press_key(0,0);
    run_one_scan_loop();
    run_one_scan_loop();
    release_key(0,0);
    for (int i=0; i < LLS_DELAY - 2; i++) { 
        run_one_scan_loop();
    }
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(KC_A))).Times(1);
    run_one_scan_loop();
    Mock::VerifyAndClearExpectations(&driver);
    // Not key up yet
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport())).Times(0);
    run_one_scan_loop();
    // But now
    EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport())).Times(1);
    run_one_scan_loop();
}

TEST_F(LLSwitch, ALayerSwitchIsInstant) {
    TestDriver driver;
    EXPECT_CALL(driver, send_keyboard_mock(_)).Times(AnyNumber());
    press_key(1,0);
    run_one_scan_loop();
    EXPECT_EQ(1 << 1, layer_state);
}

