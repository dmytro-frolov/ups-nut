#include <Arduino.h>
#include <unity.h>

#include "ups_state.h"

void setUp() {}

void tearDown() {}

static void test_ups_status_reports_online_when_red_led_is_off() {
  const UpsState state = {
      false,
      false,
      false,
  };

  TEST_ASSERT_EQUAL_STRING("OL", upsStatus(state).c_str());
}

static void test_ups_status_reports_on_battery_when_red_led_is_on() {
  const UpsState state = {
      false,
      false,
      true,
  };

  TEST_ASSERT_EQUAL_STRING("OB DISCHRG", upsStatus(state).c_str());
}

static void test_ups_status_ignores_green_and_yellow_leds() {
  const UpsState state = {
      true,
      true,
      false,
  };

  TEST_ASSERT_EQUAL_STRING("OL", upsStatus(state).c_str());
}

static int runUnityTests() {
  UNITY_BEGIN();
  RUN_TEST(test_ups_status_reports_online_when_red_led_is_off);
  RUN_TEST(test_ups_status_reports_on_battery_when_red_led_is_on);
  RUN_TEST(test_ups_status_ignores_green_and_yellow_leds);
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
