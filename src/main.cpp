#include <Arduino.h>

#include "http_api.h"
#include "nut_server.h"
#include "ups_state.h"
#include "wifi_setup.h"

void setup() {
  Serial.begin(115200);
  delay(500);

  setupUpsPins();
  connectWifi();

  setupHttpApi();
  setupNutServer();

  Serial.println("HTTP server: port 80");
  Serial.println("NUT server: port 3493, UPS name: qnapups");
}

void loop() {
  handleHttpApi();
  handleNutServer();
}
