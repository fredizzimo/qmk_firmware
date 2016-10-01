#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
    unsigned int duration;
    unsigned int current_frame_time;
} effect_param_t;

typedef void (*effect_update_func_t)(const effect_param_t* param);

typedef struct {
    unsigned int duration; // The duration of the effect in milliseconds
    effect_update_func_t update; // The update function
} effect_frame_t;

typedef struct {
    effect_frame_t* frames;
    int32_t time_left_in_frame;
} effect_runtime_t;


void add_effect(effect_runtime_t* runtime, effect_frame_t* frames, unsigned int frames_size);
void update_effects(unsigned int dt);
