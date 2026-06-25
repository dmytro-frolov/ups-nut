#include <Arduino.h>
#include <unity.h>

#include "config.h"
#include "nut_logic.h"

void setUp() {}

void tearDown() {}

static void test_supported_ups_names_include_current_and_legacy_names() {
  TEST_ASSERT_TRUE(isSupportedUpsName(UPS_NAME));
  TEST_ASSERT_TRUE(isSupportedUpsName(UPS_LEGACY_NAME));
  TEST_ASSERT_FALSE(isSupportedUpsName("otherups"));
}

static void test_nut_status_var_tracks_ups_state() {
  const UpsState online = {
      false,
      false,
      false,
  };
  const UpsState onBattery = {
      false,
      false,
      true,
  };

  TEST_ASSERT_EQUAL_STRING("OL", nutVarValue("ups.status", online).c_str());
  TEST_ASSERT_EQUAL_STRING("OB DISCHRG", nutVarValue("ups.status", onBattery).c_str());
}

static void test_nut_synthetic_battery_values_track_red_led() {
  const UpsState online = {
      false,
      false,
      false,
  };
  const UpsState onBattery = {
      false,
      false,
      true,
  };

  TEST_ASSERT_EQUAL_STRING("100", nutVarValue("battery.charge", online).c_str());
  TEST_ASSERT_EQUAL_STRING("1200", nutVarValue("battery.runtime", online).c_str());
  TEST_ASSERT_EQUAL_STRING("230", nutVarValue("input.voltage", online).c_str());

  TEST_ASSERT_EQUAL_STRING("50", nutVarValue("battery.charge", onBattery).c_str());
  TEST_ASSERT_EQUAL_STRING("600", nutVarValue("battery.runtime", onBattery).c_str());
  TEST_ASSERT_EQUAL_STRING("0", nutVarValue("input.voltage", onBattery).c_str());
}

static void test_unknown_nut_var_returns_empty_string() {
  const UpsState state = {
      false,
      false,
      false,
  };

  TEST_ASSERT_EQUAL_STRING("", nutVarValue("unknown.var", state).c_str());
}

static void test_nut_var_list_contains_supported_values_only() {
  const UpsState state = {
      false,
      false,
      false,
  };

  TEST_ASSERT_EQUAL_UINT(12, nutVarCount());
  for (size_t i = 0; i < nutVarCount(); i++) {
    const char *varName = nutVarNameAt(i);
    TEST_ASSERT_NOT_NULL(varName);
    TEST_ASSERT_TRUE(nutVarValue(varName, state).length() > 0);
  }

  TEST_ASSERT_NULL(nutVarNameAt(nutVarCount()));
}

static int runUnityTests() {
  UNITY_BEGIN();
  RUN_TEST(test_supported_ups_names_include_current_and_legacy_names);
  RUN_TEST(test_nut_status_var_tracks_ups_state);
  RUN_TEST(test_nut_synthetic_battery_values_track_red_led);
  RUN_TEST(test_unknown_nut_var_returns_empty_string);
  RUN_TEST(test_nut_var_list_contains_supported_values_only);
  return UNITY_END();
}

#ifdef ARDUINO
void setup() {
  delay(2000);
  runUnityTests();
}

void loop() {}
#else
int main() {
  return runUnityTests();
}
#endif
