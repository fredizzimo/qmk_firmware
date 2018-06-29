#ifndef WAIT_H
#define WAIT_H

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__AVR__)
#   include <util/delay.h>
#   define wait_ms(ms)  _delay_ms(ms)
#   define wait_us(us)  _delay_us(us)
#elif defined PROTOCOL_CHIBIOS
#   include "ch.h"
#   ifdef STM32_HSECLK
#       define SYS_FREQ STM32_HSECLK
#   elif defined(KINETIS_SYSCLK_FREQUENCY)
#       define SYS_FREQ KINETIS_SYSCLK_FREQUENCY
#   else
#       error "The system frequency is unknown in this platform"
#   endif

#   define NS22RTC(freq, nsec) (rtcnt_t)((((freq) + 999999UL) / 1000000UL) * nsec / 1000)

#   define wait_ms(ms) chThdSleepMilliseconds(ms)
#   define wait_us(us) chThdSleepMicroseconds(us)
#   define polled_wait_ns(ns) chSysPolledDelayX(NS22RTC(SYS_FREQ, ns))
#   define polled_wait_us(us) chSysPolledDelayX(US2RTC(SYS_FREQ, us))
#   define polled_wait_ms(ms) chSysPolledDelayX(MS2RTC(SYS_FREQ, ms))
#elif defined(__arm__)
#   include "wait_api.h"
#else  // Unit tests
void wait_ms(uint32_t ms);
#define wait_us(us) wait_ms(us / 1000)
#endif

#ifdef __cplusplus
}
#endif

#endif
