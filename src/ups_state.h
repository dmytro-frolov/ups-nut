#pragma once

#include <Arduino.h>

struct UpsState {
  bool greenLed;
  bool yellowLed;
  bool redLed;
};

void setupUpsPins();
UpsState readUpsState();
String upsStatus(const UpsState &state);
