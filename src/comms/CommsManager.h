#pragma once
#include <ArduinoJson.h>
/**
 * CommsManager is an abstract base class that defines the interface for communication managers in the project.
 * It provides pure virtual methods for initializing the communication, handling the main loop, sending telemetry data, and publishing messages to a topic.
 * This class allows for different communication implementations (e.g., MQTT, LoRaWAN) to be used interchangeably in the main application code by adhering to this common interface.
 * Each derived class must implement the following methods: 
 * - begin(): Initializes the communication manager (e.g., sets up connections, configures settings).
 * - loop(): Contains the logic that needs to be executed regularly (e.g., maintaining connections, processing incoming messages).
 * - sendTelemetry(const JsonDocument& doc): Encodes and sends telemetry data represented as a JSON document to the appropriate destination (e.g., MQTT topic, LoRaWAN uplink).
 * - publish(const char* topic, const JsonDocument& doc): Publishes a JSON document to a specified topic. The implementation may vary depending on the communication protocol (e.g., MQTT topics vs. LoRaWAN payloads).     
 * This design promotes modularity and separation of concerns, allowing the main application logic to remain agnostic of the underlying communication mechanism. 
 * It also facilitates testing and future extensions, as new communication methods can be added by simply creating new classes that inherit from CommsManager and implement the required methods.
 *      
 */
class CommsManager {
public:
    virtual ~CommsManager() = default;
    virtual void begin() = 0;
    virtual void loop() = 0;
    virtual void sendTelemetry(const JsonDocument& doc) = 0;
    virtual void publish(const char* topic, const JsonDocument& doc) = 0;
};
