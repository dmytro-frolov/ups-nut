#include "http_api.h"

#include <WebServer.h>

#include "config.h"
#include "ups_state.h"

static WebServer httpServer(80);

static String boolString(bool value) {
  return value ? "true" : "false";
}

static String jsonEscape(const String &value) {
  String escaped;
  escaped.reserve(value.length() + 8);

  for (size_t i = 0; i < value.length(); i++) {
    const char c = value[i];
    if (c == '"' || c == '\\') {
      escaped += '\\';
    }
    escaped += c;
  }

  return escaped;
}

static void handleHttpStatus() {
  const UpsState state = readUpsState();
  const String status = upsStatus(state);

  String body = "{";
  body += "\"ups\":\"" + String(UPS_NAME) + "\",";
  body += "\"status\":\"" + jsonEscape(status) + "\",";
  body += "\"green_led\":" + boolString(state.greenLed) + ",";
  body += "\"yellow_led\":" + boolString(state.yellowLed) + ",";
  body += "\"red_led\":" + boolString(state.redLed) + ",";
  body += "\"pins\":{";
  body += "\"green_led\":" + String(PIN_LED_GREEN) + ",";
  body += "\"yellow_led\":" + String(PIN_LED_YELLOW) + ",";
  body += "\"red_led\":" + String(PIN_LED_RED);
  body += "}}";

  httpServer.send(200, "application/json", body);
}

static void handleHttpRoot() {
  httpServer.send(200, "text/plain", "ESP32-C3 GPIO UPS\nGET /status\nNUT: tcp/3493, ups name: qnapups\n");
}

void setupHttpApi() {
  httpServer.on("/", handleHttpRoot);
  httpServer.on("/status", handleHttpStatus);
  httpServer.begin();
}

void handleHttpApi() {
  httpServer.handleClient();
}
