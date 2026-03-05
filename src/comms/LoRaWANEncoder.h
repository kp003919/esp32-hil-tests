#pragma once
#include <ArduinoJson.h>
#include <stdint.h>
/**
 * LoRaWANEncoder is responsible for encoding JSON telemetry data into a compact binary format suitable for transmission over LoRaWAN.
 * It defines a structure for the LoRaWAN payload and provides a function to convert a JSON document into this binary format. The encoding process should be designed to minimize the payload size while preserving the necessary information for the backend to decode and use the telemetry data effectively.
 * The encodeLoRaPayload function takes a JsonDocument as input and returns a LoRaPayload               
 * structure containing the encoded bytes and the length of the payload. The specific encoding scheme can be customized based on the requirements of the application and the constraints of the LoRaWAN network (e.g., maximum payload size, required fields, etc.).
 * This module abstracts away the details of how the JSON data is converted into a binary format,   
 *  allowing the main application code to simply call encodeLoRaPayload with the telemetry data and send the resulting payload over LoRaWAN without needing to worry about the encoding details.
 *      
 * 
 */

 // LoRaWAN payload structure   
 // - bytes: The actual binary data to be sent over LoRaWAN. The size should be kept within the limits of the LoRaWAN payload (typically 51 bytes for EU868).
 // - length: The actual length of the payload in bytes, which may be less than the maximum size of the bytes array. This allows for variable-length payloads depending on the amount of data being sent.   

struct LoRaPayload {
    uint8_t bytes[12];
    uint8_t length = 12;
};

// Function prototype for encoding a JSON document into a LoRaWAN payload. The implementation of this function will depend on the specific encoding scheme chosen for the application, which should aim to minimize the payload size while preserving the necessary information from the JSON document. The function should take a JsonDocument as input and return a LoRaPayload containing the encoded binary data and its length.
    
LoRaPayload encodeLoRaPayload(const JsonDocument& doc);
