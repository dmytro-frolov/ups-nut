#include "ups_state.h"

#include "config.h"

void setupUpsPins() {
  pinMode(PIN_LED_GREEN, INPUT_PULLUP);
  pinMode(PIN_LED_YELLOW, INPUT_PULLUP);
  pinMode(PIN_LED_RED, INPUT_PULLUP);
}

UpsState readUpsState() {
  return {
      digitalRead(PIN_LED_GREEN) == LOW,
      digitalRead(PIN_LED_YELLOW) == LOW,
      digitalRead(PIN_LED_RED) == LOW,
  };
}

String upsStatus(const UpsState &state) {
  if (state.redLed) {
    return "OB DISCHRG";
  }
  return "OL";
}
