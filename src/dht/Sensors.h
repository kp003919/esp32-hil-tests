#pragma once

#include <ArduinoJson.h>
/**
 * Sensors Interface
 * This module provides an interface for initializing and reading from the DHT sensor.
 */
void Sensors_begin();
void Sensors_read(JsonDocument& doc);
