#pragma once
#include <ArduinoJson.h>
#include "config.h"
// Initialize RTLS module
// Update RTLS state (call in main loop)
// Fill JSON document with RTLS data    
// such as location, distance, etc. 
// depending on RTLS capabilities
// Parameters:
//   doc - JSON document to populate with RTLS data 
// Functions:   
void RTLS_begin();
void RTLS_update();
void RTLS_fill(JsonDocument& doc);
