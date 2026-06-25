#include <Arduino.h>

#include "http_api.h"
#include "nut_server.h"
#include "ota_update.h"
#include "ups_state.h"
#include "wifi_setup.h"

#ifndef UNIT_TEST
void setup() {
  Serial.begin(115200);
  delay(500);

  setupUpsPins();
  connectWifi();

  setupHttpApi();
  setupNutServer();
  setupOtaUpdate();

  Serial.println("HTTP server: port 80");
  Serial.println("NUT server: port 3493, UPS name: qnapups");
  Serial.println("OTA hostname: esp32-ups-nut");
}

void loop() {
  handleWifi();
  handleHttpApi();
  handleNutServer();
  handleOtaUpdate();
}
#endif
