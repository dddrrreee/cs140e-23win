#include "rpi.h"
#include "watchdog.h"

enum {
    PM_RSTC = 0x2010001c,
    PM_RSTS = 0x20100020,
    PM_WDOG = 0x20100024,
    PM_PASSWORD = 0x5a000000,
    PM_RSTC_WRCFG_FULL_RESET = 0x00000020,
    PM_RSTC_RESET = 0x00000102,
    PM_RSTS_RASPBERRYPI_HALT = 0x555 
};

static unsigned heartbeat = 0;

void watchdog_start_ticks(unsigned ticks) {
  heartbeat = ticks;
  PUT32(PM_WDOG, PM_PASSWORD | (0xffffff & heartbeat));
  PUT32(PM_RSTC, PM_PASSWORD | PM_RSTC_WRCFG_FULL_RESET);
}

void watchdog_start_us(unsigned us) {
  watchdog_start_ticks(us / 16);
}

void watchdog_start_ms(unsigned ms) {
  watchdog_start_us(ms * 1000);
}

void watchdog_start_sec(unsigned sec) {
  watchdog_start_ms(sec * 1000);
}

int watchdog_is_running(void) {
  return GET32(PM_RSTC) & PM_RSTC_WRCFG_FULL_RESET;
}

void watchdog_stop(void) {
  PUT32(PM_RSTC, PM_PASSWORD | PM_RSTC_RESET);
}

void watchdog_resume(void) {
  PUT32(PM_RSTC, PM_PASSWORD | PM_RSTC_WRCFG_FULL_RESET);
}

unsigned watchdog_get_time_left_ticks(void) {
  return GET32(PM_WDOG) & 0xffffff;
}

unsigned watchdog_get_time_left_us(void) {
  return watchdog_get_time_left_ticks() * 16;
}

unsigned watchdog_get_time_left_ms(void) {
  return watchdog_get_time_left_us() / 1000;
}

unsigned watchdog_get_time_left_sec(void) {
  return watchdog_get_time_left_ms() / 1000;
}

void watchdog_reboot(void) {
  watchdog_start_ticks(1);
}

void watchdog_shutdown(void) {
  PUT32(PM_RSTS, GET32(PM_RSTS | PM_PASSWORD | PM_RSTS_RASPBERRYPI_HALT));
  watchdog_reboot();
}

void watchdog_feed(void) {
  assert(heartbeat);
  watchdog_start_ticks(heartbeat);
}
