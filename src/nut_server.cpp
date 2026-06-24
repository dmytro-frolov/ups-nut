#include "nut_server.h"

#include <WiFi.h>

#include "config.h"
#include "ups_state.h"

static WiFiServer nutServer(3493);

static bool isSupportedUpsName(const String &upsName) {
  return upsName == UPS_NAME || upsName == UPS_LEGACY_NAME;
}

static String commandPrefix(const String &command, const String &upsName) {
  return command + " " + upsName;
}

static String commandPrefixWithVar(const String &command, const String &upsName) {
  return commandPrefix(command, upsName) + " ";
}

static String nutVarValue(const String &varName, const UpsState &state) {
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

static void nutSendVar(WiFiClient &client, const String &varName, const UpsState &state) {
  const String value = nutVarValue(varName, state);
  if (value.length() == 0) {
    client.println("ERR VAR-NOT-SUPPORTED");
    return;
  }

  client.printf("VAR %s %s \"%s\"\n", UPS_NAME, varName.c_str(), value.c_str());
}

static void nutListVars(WiFiClient &client, const UpsState &state) {
  static const char *vars[] = {
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

  client.printf("BEGIN LIST VAR %s\n", UPS_NAME);
  for (const char *varName : vars) {
    const String value = nutVarValue(varName, state);
    client.printf("VAR %s %s \"%s\"\n", UPS_NAME, varName, value.c_str());
  }
  client.printf("END LIST VAR %s\n", UPS_NAME);
}

static String readNutLine(WiFiClient &client) {
  String line = client.readStringUntil('\n');
  line.trim();
  return line;
}

static void handleNutClient(WiFiClient &client) {
  client.setTimeout(250);

  const unsigned long deadline = millis() + 15000;
  while ((client.connected() || client.available()) && millis() < deadline) {
    if (!client.available()) {
      delay(10);
      continue;
    }

    const String line = readNutLine(client);
    if (line.length() == 0) {
      continue;
    }

    const UpsState state = readUpsState();
    const String upper = line;

    if (upper == "VER") {
      client.println("Network UPS Tools upsd 2.8.0 - ESP32 GPIO");
    } else if (upper == "NETVER") {
      client.println("1.3");
    } else if (upper == "LIST UPS") {
      client.println("BEGIN LIST UPS");
      client.printf("UPS %s \"%s\"\n", UPS_NAME, UPS_DESC);
      client.println("END LIST UPS");
    } else if (upper.startsWith("GET UPSDESC ")) {
      const String upsName = line.substring(String("GET UPSDESC ").length());
      if (isSupportedUpsName(upsName)) {
        client.printf("UPSDESC %s \"%s\"\n", upsName.c_str(), UPS_DESC);
      } else {
        client.println("ERR UPS-NOT-SUPPORTED");
      }
    } else if (upper.startsWith("LIST VAR ")) {
      const String upsName = line.substring(String("LIST VAR ").length());
      if (isSupportedUpsName(upsName)) {
        nutListVars(client, state);
      } else {
        client.println("ERR UPS-NOT-SUPPORTED");
      }
    } else if (upper.startsWith(commandPrefixWithVar("GET VAR", UPS_NAME)) || upper.startsWith(commandPrefixWithVar("GET VAR", UPS_LEGACY_NAME))) {
      const String prefix = upper.startsWith(commandPrefixWithVar("GET VAR", UPS_NAME))
                                ? commandPrefixWithVar("GET VAR", UPS_NAME)
                                : commandPrefixWithVar("GET VAR", UPS_LEGACY_NAME);
      const String varName = line.substring(prefix.length());
      nutSendVar(client, varName, state);
    } else if (upper.startsWith("GET NUMLOGINS ")) {
      const String upsName = line.substring(String("GET NUMLOGINS ").length());
      if (isSupportedUpsName(upsName)) {
        client.printf("NUMLOGINS %s 1\n", upsName.c_str());
      } else {
        client.println("ERR UPS-NOT-SUPPORTED");
      }
    } else if (upper.startsWith("LIST RW ")) {
      const String upsName = line.substring(String("LIST RW ").length());
      if (isSupportedUpsName(upsName)) {
        client.printf("BEGIN LIST RW %s\n", upsName.c_str());
        client.printf("END LIST RW %s\n", upsName.c_str());
      } else {
        client.println("ERR UPS-NOT-SUPPORTED");
      }
    } else if (upper.startsWith("LIST CMD ")) {
      const String upsName = line.substring(String("LIST CMD ").length());
      if (isSupportedUpsName(upsName)) {
        client.printf("BEGIN LIST CMD %s\n", upsName.c_str());
        client.printf("END LIST CMD %s\n", upsName.c_str());
      } else {
        client.println("ERR UPS-NOT-SUPPORTED");
      }
    } else if (upper.startsWith("USERNAME ") || upper.startsWith("PASSWORD ") || upper == commandPrefix("LOGIN", UPS_NAME) || upper == commandPrefix("LOGIN", UPS_LEGACY_NAME)) {
      client.println("OK");
    } else if (upper == "LOGOUT") {
      client.println("OK Goodbye");
      break;
    } else {
      client.println("ERR UNKNOWN-COMMAND");
    }
  }

  client.stop();
}

void setupNutServer() {
  nutServer.begin();
}

void handleNutServer() {
  WiFiClient nutClient = nutServer.available();
  if (nutClient) {
    handleNutClient(nutClient);
  }
}
