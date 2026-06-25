#pragma once

#include <Arduino.h>

#include "ups_state.h"

bool isSupportedUpsName(const String &upsName);
String nutVarValue(const String &varName, const UpsState &state);
size_t nutVarCount();
const char *nutVarNameAt(size_t index);
