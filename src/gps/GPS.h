#pragma once
#include <ArduinoJson.h>
/**
 * GPS Interface
 * This module provides an interface for initializing, updating, and filling GPS data into a JSON document.
 */
void GPS_begin();
void GPS_update();
void GPS_fill(JsonDocument& doc);
