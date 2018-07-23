#pragma once

#include "unique_id.h"

#ifndef MULTILAYOUT
#error "MULTILAYOUT not defined"
#endif

#define MLFE1(func, name, type) MULTILAYOUT_##func(name, type)
#define MLFE2(func, name, type, ...) MULTILAYOUT_##func(name, type) MLFE1(func, __VA_ARGS__)
#define MLFE3(func, name, type, ...) MULTILAYOUT_##func(name, type) MLFE2(func, __VA_ARGS__)
#define MLFE4(func, name, type, ...) MULTILAYOUT_##func(name, type) MLFE3(func, __VA_ARGS__)
#define MLFE5(func, name, type, ...) MULTILAYOUT_##func(name, type) MLFE4(func, __VA_ARGS__)
#define MLFE6(func, name, type, ...) MULTILAYOUT_##func(name, type) MLFE5(func, __VA_ARGS__)
#define MLFE7(func, name, type, ...) MULTILAYOUT_##func(name, type) MLFE6(func, __VA_ARGS__)
#define MLFE8(func, name, type, ...) MULTILAYOUT_##func(name, type) MLFE7(func, __VA_ARGS__)
#define MLFE9(func, name, type, ...) MULTILAYOUT_##func(name, type) MLFE8(func, __VA_ARGS__)

#define MULTILAYOUT_FOREACH_HELPER2(\
  _1n, _1t,\
  _2n, _2t,\
  _3n, _3t,\
  _4n, _4t,\
  _5n, _5t,\
  _6n, _6t,\
  _7n, _7t,\
  _8n, _8t,\
  _9n, _9t,\
  Name, Type, ...) Name

#define MULTILAYOUT_FOREACH_HELPER1(func, ...) \
  MULTILAYOUT_FOREACH_HELPER2(__VA_ARGS__, \
    MLFE9, invalid,\
    MLFE8, invalid,\
    MLFE7, invalid,\
    MLFE6, invalid,\
    MLFE5, invalid,\
    MLFE4, invalid,\
    MLFE3, invalid,\
    MLFE2, invalid,\
    MLFE1, invalid,)(func,__VA_ARGS__)

#define MULTILAYOUT_FOREACH(func) MULTILAYOUT_FOREACH_HELPER1(func, MULTILAYOUT)


#define ortho_4x6_ROWS 4
#define ortho_4x6_COLS 6

#define MULTILAYOUT_MATRIX(name, type) \
  uint16_t name[type##_ROWS][type##_COLS];

#define MULTILAYOUT_CONFIGURATION(name, type) \
  multilayout_module_configuration_t name;

#define MULTILAYOUT_STATUS(name, type) \
  multilayout_module_status_t name;

typedef struct {
  MULTILAYOUT_FOREACH(MATRIX)
} multilayout_t;

typedef struct {
  unique_id_t id;
} multilayout_module_configuration_t;

typedef struct {
  uint8_t keyboard_index;
  bool connected;
} multilayout_module_status_t;

typedef struct {
  union {
    struct {
      MULTILAYOUT_FOREACH(CONFIGURATION)
    };
    multilayout_module_configuration_t configurations[0];
  };
} multilayout_configuration_t;

typedef struct {
  union {
    struct {
      MULTILAYOUT_FOREACH(STATUS)
    };
    multilayout_module_status_t statuses[0];
  };
} multilayout_status_t;

extern multilayout_configuration_t multilayout_configuration;
extern multilayout_status_t multilayout_status;

static inline uint8_t get_num_multilayouts(void) {
  return sizeof(multilayout_configuration_t) / sizeof(multilayout_module_configuration_t);
}

#define NUM_MULTILAYOUT_CONFIGURATIONS \
  (sizeof(multilayout_configuration_t) / sizeof(multilayout_module_configuration_t))
