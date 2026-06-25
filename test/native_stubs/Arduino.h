#pragma once

#include <cstdint>
#include <string>

using std::size_t;

using uint8_t = std::uint8_t;

static constexpr int LOW = 0;
static constexpr int INPUT_PULLUP = 0x05;

class String {
 public:
  String() = default;
  String(const char *value) : value_(value == nullptr ? "" : value) {}
  String(const std::string &value) : value_(value) {}

  const char *c_str() const {
    return value_.c_str();
  }

  size_t length() const {
    return value_.length();
  }

  bool operator==(const char *other) const {
    return value_ == (other == nullptr ? "" : other);
  }

  bool operator==(const String &other) const {
    return value_ == other.value_;
  }

 private:
  std::string value_;
};

inline bool operator==(const char *lhs, const String &rhs) {
  return rhs == lhs;
}

inline void delay(unsigned long) {}
inline void pinMode(uint8_t, int) {}
inline int digitalRead(uint8_t) {
  return LOW;
}
