#include <Arduino.h>
#include <ArduinoJson.h>
#include "hil_test_mode.h"
#include "config.h"

// ---------- DHT ----------
#include <DHT.h>

#ifndef DHTTYPE
#define DHTTYPE DHT22
#endif

DHT dht(DHTPIN, DHTTYPE);

// ---------- GPS ----------
#include <TinyGPSPlus.h>
TinyGPSPlus gps;
HardwareSerial GPS_Serial(1);  // UART1

// ---------- Globals ----------
unsigned long bootTime = 0;

// ---------- Helper functions ----------

float readTemperature() { return dht.readTemperature(); }
float readHumidity()    { return dht.readHumidity(); }

float getLat() {
    while (GPS_Serial.available()) gps.encode(GPS_Serial.read());
    return gps.location.isValid() ? gps.location.lat() : 0.0f;
}

float getLon() {
    while (GPS_Serial.available()) gps.encode(GPS_Serial.read());
    return gps.location.isValid() ? gps.location.lng() : 0.0f;
}

// ---------- Entry point ----------

void startHilTestMode() {
    Serial.println("[HIL] Test mode active");
    bootTime = millis();

    pinMode(LED_BUILTIN, OUTPUT);

    dht.begin();
    GPS_Serial.begin(9600, SERIAL_8N1, 16, 17);  // adjust pins if needed

    while (true) {
        if (!Serial.available()) {
            delay(5);
            continue;
        }

        String cmd = Serial.readStringUntil('\n');
        cmd.trim();

        // ---------- Basic ----------
        if (cmd == "PING") {
            Serial.println("[TEST] PONG");
        }
        else if (cmd == "TEST_UPTIME") {
            unsigned long uptime = (millis() - bootTime) / 1000;
            Serial.printf("[TEST] UPTIME %lu\n", uptime);
        }
        else if (cmd == "TEST_PULSE") {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(50);
            digitalWrite(LED_BUILTIN, LOW);
            Serial.println("[TEST] PULSE_DONE");
        }

        // ---------- DHT ----------
        else if (cmd == "TEST_DHT") {
            StaticJsonDocument<128> doc;
            doc["temperature"] = readTemperature();
            doc["humidity"]    = readHumidity();

            Serial.print("[TEST] ");
            serializeJson(doc, Serial);
            Serial.println();
        }

        // ---------- GPS ----------
        else if (cmd == "TEST_GPS") {
            StaticJsonDocument<128> doc;
            doc["lat"] = getLat();
            doc["lon"] = getLon();

            Serial.print("[TEST] ");
            serializeJson(doc, Serial);
            Serial.println();
        }

        // ---------- RTLS (stubbed) ----------
    else if (cmd == "TEST_RTLS") {
    StaticJsonDocument<64> doc;
    JsonArray arr = doc.createNestedArray("rtls");  // empty list
    Serial.print("[TEST] ");
    serializeJson(doc, Serial);
    Serial.println();
}


        // ---------- Unknown ----------
        else {
            Serial.println("[TEST] UNKNOWN_CMD");
        }
    }
}
