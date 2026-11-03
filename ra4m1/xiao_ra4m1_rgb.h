// Drives the XIAO RA4M1 RGB LED (WS2812): data on P112, power on P500.
// P112 must already be an output; the YAML sets that up with a gpio output.
#pragma once

#include <cstdint>

#include "esphome/core/helpers.h"

// Zephyr's devicetree for this package has no ioport5, so P500 is set directly.
// Port 5 PCNTR1: bit 0 makes P500 an output, bit 16 drives it high.
inline volatile uint32_t *const RGB_PORT5_PCNTR1 = reinterpret_cast<volatile uint32_t *>(0x400400A0);
// Port 1 PCNTR3: the low 16 bits set a pin high, the high 16 bits set it low.
inline volatile uint32_t *const RGB_PORT1_PCNTR3 = reinterpret_cast<volatile uint32_t *>(0x40040028);
inline volatile uint32_t *const RGB_DEMCR = reinterpret_cast<volatile uint32_t *>(0xE000EDFC);
inline volatile uint32_t *const RGB_DWT_CTRL = reinterpret_cast<volatile uint32_t *>(0xE0001000);
inline volatile uint32_t *const RGB_DWT_CYCCNT = reinterpret_cast<volatile uint32_t *>(0xE0001004);

static constexpr uint32_t RGB_P112_HIGH = 1u << 12;
static constexpr uint32_t RGB_P112_LOW = 1u << 28;
// Cycles at 48 MHz: 0.4 us, 0.8 us and 1.25 us.
static constexpr uint32_t RGB_T0H = 19;
static constexpr uint32_t RGB_T1H = 38;
static constexpr uint32_t RGB_PERIOD = 60;

// Powers the LED and starts the DWT cycle counter that times the pulses.
inline void rgb_init() {
  *RGB_PORT5_PCNTR1 |= 0x00010001u;
  *RGB_DEMCR |= 1u << 24;
  *RGB_DWT_CTRL |= 1u;
}

// Call rgb_init() first: the wait loops never end if the cycle counter is not running.
inline void rgb_set(uint8_t red, uint8_t green, uint8_t blue) {
  // WS2812 takes green, red, blue, most significant bit first.
  const uint8_t grb[3] = {green, red, blue};
  esphome::InterruptLock lock;
  for (uint8_t value : grb) {
    for (uint8_t mask = 0x80; mask != 0; mask >>= 1) {
      const uint32_t high = (value & mask) ? RGB_T1H : RGB_T0H;
      const uint32_t start = *RGB_DWT_CYCCNT;
      *RGB_PORT1_PCNTR3 = RGB_P112_HIGH;
      while (*RGB_DWT_CYCCNT - start < high) {
      }
      *RGB_PORT1_PCNTR3 = RGB_P112_LOW;
      while (*RGB_DWT_CYCCNT - start < RGB_PERIOD) {
      }
    }
  }
}
