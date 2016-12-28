#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <tuple>

using namespace testing;

extern "C" {
    #include "quantum/effect/effect.h"
}

class EffectTests : public testing::Test {
public:
    EffectTests() {
        mock = &mock_instance;
        // Reset the current time to zero
        update_effects(0);
    }

    ~EffectTests() {
        mock = nullptr;
        clear_all_effects();
    }

    static void update1(const effect_param_t* param) {
        mock->update1(param);
    }

    static void update2(const effect_param_t* param) {
        mock->update2(param);
    }

    class update_mock_t {
    public:
        MOCK_METHOD1(update1, void (const effect_param_t* param));
        MOCK_METHOD1(update2, void (const effect_param_t* param));
    };

    static update_mock_t* mock;
private:
    update_mock_t mock_instance;
};

EffectTests::update_mock_t* EffectTests::mock = nullptr;

TEST_F(EffectTests, UpdateFirstFrame_ShouldCallUpdateWithCorrectParameters) {
    effect_frame_t frames[] = {
        {
            .duration = 10,
            .update = update1
        }
    };
    int userdata = 5;

    add_effect(0, frames, sizeof(frames), &userdata, sizeof(userdata), EFFECT_NO_LOOP);
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::duration, 10),
        Field(&effect_param_t::current_frame_time, 1),
        Field(&effect_param_t::current_frame_nr, 0),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(userdata)))
    )));
    update_effects(1);
}

TEST_F(EffectTests, UpdateFirstFrameOnAnotherSlot_ShouldCallUpdateWithCorrectParameters) {
    effect_frame_t frames[] = {
        {
            .duration = 10,
            .update = update1
        }
    };
    int userdata = 5;

    add_effect(5, frames, sizeof(frames), &userdata, sizeof(userdata), EFFECT_NO_LOOP);
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::duration, 10),
        Field(&effect_param_t::current_frame_time, 1),
        Field(&effect_param_t::current_frame_nr, 0),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(userdata)))
    )));
    update_effects(1);
}

TEST_F(EffectTests, UpdateEffectThatHasEnded_ShouldCallNothing) {
    effect_frame_t frames[] = {
        {
            .duration = 5,
            .update = update1
        }
    };

    add_effect(0, frames, sizeof(frames), NULL, 0, EFFECT_NO_LOOP);
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 5))).Times(1);
    update_effects(5);
    update_effects(6);
}

TEST_F(EffectTests, UpdateEffectWithDtLessThanDuration_ShouldAllowTheSameEffectToBeUpdatedAgain) {
    effect_frame_t frames[] = {
        {
            .duration = 5,
            .update = update1
        }
    };

    add_effect(0, frames, sizeof(frames), NULL, 0, EFFECT_NO_LOOP);
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 4)));
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 5)));
    update_effects(4);
    update_effects(5);
}

TEST_F(EffectTests, UpdateEffectWithDtLongerThanDuration_ShouldCallUpdateAndEndTheEffect) {
    effect_frame_t frames[] = {
        {
            .duration = 5,
            .update = update1
        }
    };

    add_effect(0, frames, sizeof(frames), NULL, 0, EFFECT_NO_LOOP);
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 5)));
    update_effects(6);
    update_effects(7);
}

TEST_F(EffectTests, UpdateSameEffectMultipleTimes_ShouldAllowUpdateToBeCalledForEachOfThem) {
    effect_frame_t frames[] = {
        {
            .duration = 10,
            .update = update1
        }
    };

    add_effect(0, frames, sizeof(frames), NULL, 0, EFFECT_NO_LOOP);
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 1)));
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 4)));
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 9)));
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 10)));
    update_effects(1);
    update_effects(4);
    update_effects(9);
    update_effects(13);
    // The effect should have ended here
    update_effects(14);
}

TEST_F(EffectTests, UpdateEffectWithZeroDt_ShouldDoNothing) {
    effect_frame_t frames[] = {
        {
            .duration = 5,
            .update = update1
        }
    };

    add_effect(0, frames, sizeof(frames), NULL, 0, EFFECT_NO_LOOP);
    EXPECT_CALL(*mock, update1(_)).Times(0);
    update_effects(0);
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 1)));
    update_effects(1);
}


TEST_F(EffectTests, UpdateSecondFrameFromStart_ShouldUpdateItWithTheCorrectParameters) {
    effect_frame_t frames[] = {
        {
            .duration = 10,
            .update = update1
        },
        {
            .duration = 5,
            .update = update2
        }
    };
    int userdata = 3;
    add_effect(0, frames, sizeof(frames), &userdata, sizeof(userdata), EFFECT_NO_LOOP);
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::duration, 10),
        Field(&effect_param_t::current_frame_time, 10),
        Field(&effect_param_t::current_frame_nr, 0),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(userdata)))
    )));
    update_effects(10);
    EXPECT_CALL(*mock, update2(AllOf(
        Field(&effect_param_t::duration, 5),
        Field(&effect_param_t::current_frame_time, 1),
        Field(&effect_param_t::current_frame_nr, 1),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(userdata)))
    )));
    update_effects(11);
}

TEST_F(EffectTests, UpdateFirstFrameWithExcessTime_ShouldUpdateTheSecondFrameWithTheRemainder) {
    effect_frame_t frames[] = {
        {
            .duration = 5,
            .update = update1
        },
        {
            .duration = 10,
            .update = update2
        }
    };
    add_effect(0, frames, sizeof(frames), NULL, 0, EFFECT_NO_LOOP);
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 5)));
    EXPECT_CALL(*mock, update2(Field(&effect_param_t::current_frame_time, 6)));
    update_effects(11);
}

TEST_F(EffectTests, UpdatingWithBigEnoughDt_ShouldUpdateAllTheFramesBetween) {
    effect_frame_t frames[] = {
        {
            .duration = 5,
            .update = update1
        },
        {
            .duration = 10,
            .update = update2
        },
        {
            .duration = 10,
            .update = update2
        }
    };
    add_effect(0, frames, sizeof(frames), NULL, 0, EFFECT_NO_LOOP);
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 5)));
    EXPECT_CALL(*mock, update2(Field(&effect_param_t::current_frame_time, 10))).Times(2);
    update_effects(26);
    update_effects(27);
}

TEST_F(EffectTests, UpdatingInstantFrame_ShouldMoveToTheNextFrameWithoutUsingTime) {
    effect_frame_t frames[] = {
        {
            // In c the duration can be left out complete, but c++ requires it
            .duration = 0,
            .update = update1
        },
        {
            .duration = 10,
            .update = update2
        },
    };
    add_effect(0, frames, sizeof(frames), NULL, 0, EFFECT_NO_LOOP);
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 0)));
    EXPECT_CALL(*mock, update2(Field(&effect_param_t::current_frame_time, 2)));
    update_effects(2);
}

TEST_F(EffectTests, UpdatingInstantFrameAfterNormalWithExactTime_IsUpdatedToo) {
    effect_frame_t frames[] = {
        {
            .duration = 10,
            .update = update1
        },
        {
            .duration = 0,
            .update = update2
        },
    };
    add_effect(0, frames, sizeof(frames), NULL, 0, EFFECT_NO_LOOP);
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 10)));
    EXPECT_CALL(*mock, update2(Field(&effect_param_t::current_frame_time, 0)));
    update_effects(10);
    Mock::VerifyAndClearExpectations(mock);
    update_effects(20);
}

TEST_F(EffectTests, OneFrameInfiniteLoopingEffect_ShouldLoopCorrectly) {
    effect_frame_t frames[] = {
        {
            .duration = 5,
            .update = update1
        },
    };
    add_effect(0, frames, sizeof(frames), NULL, 0, EFFECT_LOOP_INFINITE);
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 5)));
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 1)));
    update_effects(6);
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 5))).Times(3);
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 1)));
    update_effects(21);
}

TEST_F(EffectTests, OneFrameFixedLoop_ShouldLoopCorrectly) {
    effect_frame_t frames[] = {
        {
            .duration = 5,
            .update = update1
        },
    };
    add_effect(0, frames, sizeof(frames), NULL, 0, 2);
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 1)));
    update_effects(1);
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 5))).Times(2);
    update_effects(11);
    update_effects(13);
}

TEST_F(EffectTests, TwoFrameFixedLoop_ShouldLoopCorrectly) {
    effect_frame_t frames[] = {
        {
            .duration = 5,
            .update = update1
        },
        {
            .duration = 0,
            .update = update2
        },
    };
    add_effect(0, frames, sizeof(frames), NULL, 0, 3);
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 5)));
    EXPECT_CALL(*mock, update2(Field(&effect_param_t::current_frame_time, 0)));
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 1)));
    update_effects(6);
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 5))).Times(2);
    EXPECT_CALL(*mock, update2(Field(&effect_param_t::current_frame_time, 0))).Times(2);
    update_effects(16);
    Mock::VerifyAndClear(mock);
    update_effects(26);
}

TEST_F(EffectTests, EffectEntryAndExit_ShouldBeSetCorrectly) {
    effect_frame_t frames[] = {
        {
            .duration = 5,
            .update = update1
        },
    };
    add_effect(0, frames, sizeof(frames), NULL, 0, EFFECT_NO_LOOP);
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::entry, true),
        Field(&effect_param_t::exit, false)
    )));
    update_effects(2);
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::entry, false),
        Field(&effect_param_t::exit, false)
    )));
    update_effects(4);
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::entry, false),
        Field(&effect_param_t::exit, true)
    )));
    update_effects(6);
}

TEST_F(EffectTests, EffectEntryAndExit_CanBeSetAtTheSameTime) {
    effect_frame_t frames[] = {
        {
            .duration = 5,
            .update = update1
        },
    };
    add_effect(0, frames, sizeof(frames), NULL, 0, EFFECT_NO_LOOP);
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::entry, true),
        Field(&effect_param_t::exit, true)
    )));
    update_effects(5);
}

TEST_F(EffectTests, UpdateTwoEffects_ShouldNotAffectEachOther) {
    effect_frame_t frames1[] = {
        {
            .duration = 5,
            .update = update1
        },
    };
    effect_frame_t frames2[] = {
        {
            .duration = 10,
            .update = update2
        },
    };
    int data1 = 1;
    int data2 = 2;
    add_effect(0, frames1, sizeof(frames1), &data1, sizeof(data1), EFFECT_NO_LOOP);
    add_effect(1, frames2, sizeof(frames2), &data2, sizeof(data2), EFFECT_NO_LOOP);
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::current_frame_time, 2),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data1)))
    )));
    EXPECT_CALL(*mock, update2(AllOf(
        Field(&effect_param_t::current_frame_time, 2),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data2)))
    )));
    update_effects(2);
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::current_frame_time, 5)));
    EXPECT_CALL(*mock, update2(Field(&effect_param_t::current_frame_time, 6)));
    update_effects(6);
    EXPECT_CALL(*mock, update2(Field(&effect_param_t::current_frame_time, 10)));
    update_effects(12);
}


TEST_F(EffectTests, UpdateTwoEffectsWithSameFrames_ShouldGetDifferentUserData) {
    effect_frame_t frames[] = {
        {
            .duration = 5,
            .update = update1
        },
    };
    int data1 = 1;
    int data2 = 2;
    add_effect(1, frames, sizeof(frames), &data1, sizeof(data1), EFFECT_NO_LOOP);
    add_effect(2, frames, sizeof(frames), &data2, sizeof(data2), EFFECT_NO_LOOP);
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::current_frame_time, 2),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data1)))
    )));
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::current_frame_time, 2),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data2)))
    )));
    update_effects(2);
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::current_frame_time, 5),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data1)))
    )));
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::current_frame_time, 5),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data2)))
    )));
    update_effects(6);
}

TEST_F(EffectTests, RemoveFirstEffect_ShouldNotUpdateItAnymore) {
    effect_frame_t frames[] = {
        {
            .duration = 5,
            .update = update1
        },
    };
    int data1 = 1;
    int data2 = 2;
    add_effect(0, frames, sizeof(frames), &data1, sizeof(data1), EFFECT_NO_LOOP);
    add_effect(1, frames, sizeof(frames), &data2, sizeof(data2), EFFECT_NO_LOOP);
    remove_effect(0);

    EXPECT_CALL(*mock, update1(Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data2)))));
    update_effects(1);
}

TEST_F(EffectTests, RemoveLastEffect_ShouldNotUpdateItAnymore) {
    effect_frame_t frames[] = {
        {
            .duration = 5,
            .update = update1
        },
    };
    int data1 = 1;
    int data2 = 2;
    add_effect(0, frames, sizeof(frames), &data1, sizeof(data1), EFFECT_NO_LOOP);
    add_effect(1, frames, sizeof(frames), &data2, sizeof(data2), EFFECT_NO_LOOP);
    remove_effect(1);

    EXPECT_CALL(*mock, update1(Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data1)))));
    update_effects(1);
}

TEST_F(EffectTests, RemoveMiddleEffect_ShouldStillUpdateTheOthers) {
    effect_frame_t frames[] = {
        {
            .duration = 5,
            .update = update1
        },
    };
    int data1 = 1;
    int data2 = 2;
    int data3 = 3;
    add_effect(0, frames, sizeof(frames), &data1, sizeof(data1), EFFECT_NO_LOOP);
    add_effect(1, frames, sizeof(frames), &data2, sizeof(data2), EFFECT_NO_LOOP);
    add_effect(2, frames, sizeof(frames), &data3, sizeof(data3), EFFECT_NO_LOOP);
    remove_effect(1);

    EXPECT_CALL(*mock, update1(Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data1)))));
    EXPECT_CALL(*mock, update1(Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data3)))));
    update_effects(1);
}

TEST_F(EffectTests, ReAddFirstEffect_ShouldRestartIt) {
    effect_frame_t frames[] = {
        {
            .duration = 5,
            .update = update1
        },
    };
    int data1 = 1;
    int data2 = 2;
    add_effect(0, frames, sizeof(frames), &data1, sizeof(data1), EFFECT_NO_LOOP);
    add_effect(1, frames, sizeof(frames), &data2, sizeof(data2), EFFECT_NO_LOOP);

    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::current_frame_time, 2),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data1)))
    )));
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::current_frame_time, 2),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data2)))
    )));
    update_effects(2);

    add_effect(0, frames, sizeof(frames), &data1, sizeof(data1), EFFECT_NO_LOOP);
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::current_frame_time, 3),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data1)))
    )));
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::current_frame_time, 5),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data2)))
    )));
    update_effects(5);
}

TEST_F(EffectTests, ReAddLastEffect_ShouldRestartIt) {
    effect_frame_t frames[] = {
        {
            .duration = 5,
            .update = update1
        },
    };
    int data1 = 1;
    int data2 = 2;
    add_effect(0, frames, sizeof(frames), &data1, sizeof(data1), EFFECT_NO_LOOP);
    add_effect(1, frames, sizeof(frames), &data2, sizeof(data2), EFFECT_NO_LOOP);

    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::current_frame_time, 2),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data1)))
    )));
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::current_frame_time, 2),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data2)))
    )));
    update_effects(2);

    add_effect(1, frames, sizeof(frames), &data2, sizeof(data2), EFFECT_NO_LOOP);
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::current_frame_time, 5),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data1)))
    )));
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::current_frame_time, 3),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data2)))
    )));
    update_effects(5);
}

TEST_F(EffectTests, ReAddMiddleEffect_ShouldRestartIt) {
    effect_frame_t frames[] = {
        {
            .duration = 5,
            .update = update1
        },
    };
    int data1 = 1;
    int data2 = 2;
    int data3 = 3;
    add_effect(0, frames, sizeof(frames), &data1, sizeof(data1), EFFECT_NO_LOOP);
    add_effect(1, frames, sizeof(frames), &data2, sizeof(data2), EFFECT_NO_LOOP);
    add_effect(2, frames, sizeof(frames), &data3, sizeof(data3), EFFECT_NO_LOOP);

    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::current_frame_time, 2),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data1)))
    )));
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::current_frame_time, 2),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data2)))
    )));
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::current_frame_time, 2),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data3)))
    )));
    update_effects(2);

    add_effect(1, frames, sizeof(frames), &data2, sizeof(data2), EFFECT_NO_LOOP);
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::current_frame_time, 5),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data1)))
    )));
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::current_frame_time, 3),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data2)))
    )));
    EXPECT_CALL(*mock, update1(AllOf(
        Field(&effect_param_t::current_frame_time, 5),
        Field(&effect_param_t::user_data, MatcherCast<int*>(Pointee(data3)))
    )));
    update_effects(5);
}
