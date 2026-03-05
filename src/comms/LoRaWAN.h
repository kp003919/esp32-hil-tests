#pragma once
#include "CommsManager.h"

class LoRaWANComms : public CommsManager {
public:
    void begin() override;
    void loop() override;
    void sendTelemetry(const JsonDocument& doc) override;
    void publish(const char* topic, const JsonDocument& doc) override;
};
