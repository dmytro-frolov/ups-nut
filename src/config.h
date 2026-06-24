#pragma once

#include <Arduino.h>

inline constexpr char UPS_NAME[] = "qnapups";
inline constexpr char UPS_LEGACY_NAME[] = "espups";
inline constexpr char UPS_DESC[] = "ESP32-C3 GPIO UPS";

// Short each pin to GND to simulate the corresponding UPS LED being on.
inline constexpr uint8_t PIN_LED_GREEN = 4;
inline constexpr uint8_t PIN_LED_YELLOW = 5;
inline constexpr uint8_t PIN_LED_RED = 6;
