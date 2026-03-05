#include "LoRaWANEncoder.h"

LoRaPayload encodeLoRaPayload(const JsonDocument& doc) {
    LoRaPayload p;

    float t   = doc["temperature"] | 0.0f;
    float h   = doc["humidity"]    | 0.0f;
    float lat = doc["lat"]         | 0.0f;
    float lon = doc["lon"]         | 0.0f;
    float alt = doc["alt"]         | 0.0f;
    float spd = doc["speed"]       | 0.0f;

    int16_t lat100 = (int16_t)(lat * 100);
    int16_t lon100 = (int16_t)(lon * 100);

    p.bytes[0] = (int8_t)t;
    p.bytes[1] = (uint8_t)h;

    p.bytes[2] = (lat100 >> 8) & 0xFF;
    p.bytes[3] = lat100 & 0xFF;

    p.bytes[4] = (lon100 >> 8) & 0xFF;
    p.bytes[5] = lon100 & 0xFF;

    uint16_t alt16 = (uint16_t)alt;
    p.bytes[6] = (alt16 >> 8) & 0xFF;
    p.bytes[7] = alt16 & 0xFF;

    p.bytes[8] = (uint8_t)spd;

    // ---- FINAL RTLS SECTION (ArduinoJson 6.21.x) ----
    JsonVariantConst rtlsVar = doc["rtls"];
    uint8_t rtlsCount = 0;

    if (rtlsVar.is<JsonArray>()) {
        JsonArrayConst rtls = rtlsVar.as<JsonArrayConst>();
        rtlsCount = rtls.size();
    }

    p.bytes[9]  = rtlsCount;
    p.bytes[10] = 0;
    p.bytes[11] = 0;
    // -------------------------------------------------

    return p;
}
