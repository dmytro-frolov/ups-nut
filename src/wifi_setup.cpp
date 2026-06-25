#include "wifi_setup.h"

#include <Arduino.h>
#include <WiFi.h>

#if __has_include("secrets.h")
#include "secrets.h"
#endif

#ifndef WIFI_SSID
#define WIFI_SSID "CHANGE_ME"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "CHANGE_ME"
#endif

static constexpr unsigned long WIFI_CONNECT_TIMEOUT_MS = 15000;
static constexpr unsigned long WIFI_RECONNECT_INTERVAL_MS = 10000;

static unsigned long lastReconnectAttemptMs = 0;
static wl_status_t lastReportedStatus = WL_IDLE_STATUS;

static void printWifiStatus(wl_status_t status) {
  switch (status) {
    case WL_CONNECTED:
      Serial.print("connected");
      break;
    case WL_NO_SSID_AVAIL:
      Serial.print("SSID unavailable");
      break;
    case WL_CONNECT_FAILED:
      Serial.print("connect failed");
      break;
    case WL_CONNECTION_LOST:
      Serial.print("connection lost");
      break;
    case WL_DISCONNECTED:
      Serial.print("disconnected");
      break;
    default:
      Serial.print(static_cast<int>(status));
      break;
  }
}

static void beginWifiConnect() {
  WiFi.disconnect(false);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lastReconnectAttemptMs = millis();
}

void connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);
  beginWifiConnect();

  Serial.printf("Connecting to Wi-Fi SSID '%s'", WIFI_SSID);
  const unsigned long startedAt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startedAt < WIFI_CONNECT_TIMEOUT_MS) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  lastReportedStatus = WiFi.status();
  if (lastReportedStatus == WL_CONNECTED) {
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    return;
  }

  Serial.print("Wi-Fi initial connection timed out, status: ");
  printWifiStatus(lastReportedStatus);
  Serial.println();
}

void handleWifi() {
  const wl_status_t status = WiFi.status();
  if (status != lastReportedStatus) {
    Serial.print("Wi-Fi status: ");
    printWifiStatus(status);
    if (status == WL_CONNECTED) {
      Serial.print(", IP: ");
      Serial.print(WiFi.localIP());
    }
    Serial.println();
    lastReportedStatus = status;
  }

  if (status == WL_CONNECTED) {
    return;
  }

  const unsigned long now = millis();
  if (now - lastReconnectAttemptMs < WIFI_RECONNECT_INTERVAL_MS) {
    return;
  }

  Serial.printf("Retrying Wi-Fi SSID '%s'\n", WIFI_SSID);
  beginWifiConnect();
}
