#include "unique_id.h"
#include <string.h>
#ifdef PROTOCOL_CHIBIOS
#include "ch.h"
#include "hal.h"
#endif

// Some MCUs don't define UID_BASE even if it's available
#ifndef UID_BASE
#ifdef STM32F303xC
#define UID_BASE ((uint32_t)0x1FFFF7ACU)
#endif
#endif

#ifdef UID_BASE
uint8_t* get_unique_id(void) {
  static unique_id_t id = {};
  static bool initialized = false;
  if (!initialized) {
    memcpy(id, (void*)UID_BASE, 12);
    initialized = true;
  }
  return id;
}
#endif

bool are_unique_ids_same(uint8_t* id1, uint8_t* id2) {
  return memcmp(id1, id2, sizeof(unique_id_t)) == 0;
}
