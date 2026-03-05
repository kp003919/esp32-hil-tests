#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include <ArduinoJson.h>
#include "spi_driver.h"
#include "uart_driver.h"
#include "I2C_Driver.h"

// Backend types
enum BackendType {
    BACKEND_NODE_RED = 0,
    BACKEND_AWS = 1,
    BACKEND_THINGSBOARD = 2
};

// Global backend state (shared with callback)
extern BackendType backend;
/**
 * WiFiMQTT class definition
 * This class manages the WiFi connection and MQTT communication with the backend. 
 * It provides methods to initialize the connection, maintain the MQTT loop, send telemetry
 *  data, and publish messages to specific topics. The class also includes a method to set 
 * the backend type, which can be used to switch between different communication protocols 
 * or brokers as needed. The implementation of these methods will handle the specifics of 
 * connecting to the MQTT broker, subscribing to topics, and processing incoming messages 
 * based on the defined topics and payload formats. This class is designed to be used in 
 * an IoT project where the ESP32 collects sensor data and allows remote control via MQTT
 *  messages, with support for multiple backends.
 * 
 * Note: The actual implementation of the methods will be in the corresponding .cpp file,
 *  where the logic for connecting to the MQTT broker, handling incoming messages, and sending 
 * telemetry data will be defined. The class provides a clear interface for managing MQTT
 *  communication, allowing the main application code to remain clean and focused on sensor
 *  data collection and processing. 
 * 
 */

class WiFiMQTT {
public:
    void begin();
    void loop();
    void sendTelemetry(const JsonDocument& doc);
    void publish(const char* topic, const JsonDocument& doc);
    void setBackend(BackendType b);  // implemented in .cpp
private:
    void configureMQTT();
};

#endif
