#include "uuid.h"


#ifdef UID_BASE
uint8_t* get_unique_id(void) {
  static unique_id_t id = {};
  static initialized = false;
  if (!initialized) {
    memcpy(id, (void*)UID_BASE, 12);
    initialized = true;
  }
  return id;
}
#endif
