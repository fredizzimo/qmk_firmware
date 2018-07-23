#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t unique_id_t[16];

uint8_t* get_hardware_unique_id(void);
uint8_t* get_manual_unique_id(void);
bool are_unique_ids_same(uint8_t* id1, uint8_t* id2);
void assign_unique_id(uint8_t* target, uint8_t* source);
