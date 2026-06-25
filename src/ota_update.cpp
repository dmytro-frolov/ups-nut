#include "ota_update.h"

#include <Arduino.h>
#include <ArduinoOTA.h>

#if __has_include("secrets.h")
#include "secrets.h"
#endif

void setupOtaUpdate() {
  ArduinoOTA.setHostname("esp32-ups-nut");

#ifdef OTA_PASSWORD
  ArduinoOTA.setPassword(OTA_PASSWORD);
#endif

  ArduinoOTA
      .onStart([]() {
        Serial.println("OTA start");
      })
      .onEnd([]() {
        Serial.println("OTA end");
      })
      .onError([](ota_error_t error) {
        Serial.printf("OTA error: %u\n", error);
      });

  ArduinoOTA.begin();
}

void handleOtaUpdate() {
  ArduinoOTA.handle();
}
