#include "nut_logic.h"

#include "config.h"

static const char *NUT_VAR_NAMES[] = {
    "device.mfr",
    "device.model",
    "device.type",
    "driver.name",
    "driver.version",
    "ups.mfr",
    "ups.model",
    "ups.status",
    "ups.test.result",
    "battery.charge",
    "battery.runtime",
    "input.voltage",
};

bool isSupportedUpsName(const String &upsName) {
  return upsName == UPS_NAME || upsName == UPS_LEGACY_NAME;
}

String nutVarValue(const String &varName, const UpsState &state) {
  if (varName == "device.mfr") return "DIY";
  if (varName == "device.model") return "ESP32-C3 GPIO UPS";
  if (varName == "device.type") return "ups";
  if (varName == "driver.name") return "esp32-gpio";
  if (varName == "driver.version") return "0.1.0";
  if (varName == "ups.mfr") return "DIY";
  if (varName == "ups.model") return "ESP32-C3 GPIO UPS";
  if (varName == "ups.status") return upsStatus(state);
  if (varName == "ups.test.result") return "No test initiated";
  if (varName == "battery.charge") return state.redLed ? "50" : "100";
  if (varName == "battery.runtime") return state.redLed ? "600" : "1200";
  if (varName == "input.voltage") return state.redLed ? "0" : "230";
  return "";
}

size_t nutVarCount() {
  return sizeof(NUT_VAR_NAMES) / sizeof(NUT_VAR_NAMES[0]);
}

const char *nutVarNameAt(size_t index) {
  if (index >= nutVarCount()) {
    return nullptr;
  }

  return NUT_VAR_NAMES[index];
}
