#include "LoRaWAN.h"
/**
 * LoRaWAN communication module for ESP32
 * - Uses the LMIC library to handle LoRaWAN protocol   
 * - Configured for OTAA (Over-The-Air Activation) with keys stored in program memory
 * - Provides a function to send telemetry data as a JSON document, which is encoded into a LoRaWAN payload
 * - Handles LoRaWAN events such as joining, transmission completion, and reception of downlink
 * Author: Muhsin Atto
 * Date: 2024-06-01 
 * Details:             
 * - The DEVEUI, APPEUI, and APPKEY are defined as constants in the config.h file and stored in program memory to save RAM.
 * - The onEvent function logs LoRaWAN events and schedules the next uplink transmission after a fixed interval (30 seconds) upon transmission completion.
 * - The do_send function checks if the LoRaWAN stack is ready for transmission and queues a placeholder payload. The actual telemetry data is sent via the sendTelemetry function, which encodes the JSON document into a LoRaWAN payload and queues it for transmission.
 * - Ensure that the keys are in big-endian format as required by the LoRaWAN specification and the LMIC library.
 * - Adjust the pin mapping in the lmic_pins structure according to your specific hardware setup.
 * - This module is designed to be integrated into a larger application where telemetry data is collected and sent to a LoRaWAN network, such as The Things Network or a private LoRaWAN server.
 * - Note: This code is meant for educational purposes and may require additional error handling and security considerations for production use. Always ensure that your LoRaWAN keys are kept secure and that your network server is configured correctly to accept your device's join requests.   
 *  @copyright 2024 Muhsin Atto. All rights reserved.
 *  @license MIT License (https://opensource.org/licenses/MIT)      
 * 
 */

#include "../config.h"
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "LoRaWANEncoder.h"

// LoRaWAN keys (placeholders – replace with real)  
// Stored in program memory to save RAM 
// Functions to retrieve them for LMIC  library
// Note: Ensure keys are in big-endian format as required by LoRaWAN specification  
// and LMIC library.
static const u1_t PROGMEM DEVEUI[8]  = LORAWAN_DEVEUI;
static const u1_t PROGMEM APPEUI[8]  = LORAWAN_APPEUI;
static const u1_t PROGMEM APPKEY[16] = LORAWAN_APPKEY;

void os_getDevEui(u1_t* buf) { memcpy_P(buf, DEVEUI, 8); }
void os_getArtEui(u1_t* buf) { memcpy_P(buf, APPEUI, 8); }
void os_getDevKey(u1_t* buf) { memcpy_P(buf, APPKEY, 16); }

// Adjust pins to your board later
const lmic_pinmap lmic_pins = {
    .nss = 18,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 14,
    .dio = {26, 33, 32}
};

// LoRaWANComms class implementation
static osjob_t sendjob;
static const unsigned long LORAWAN_TX_INTERVAL = 30; // seconds

static void do_send(osjob_t* j);

// LoRaWAN event handler    
// Logs events and manages uplink scheduling    
// Note: Adjust event handling as needed for application logic  
// such as join retries, downlink processing, etc.  
// This implementation focuses on basic logging and periodic uplink scheduling. 
 void onEvent(ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    switch (ev) {
        case EV_JOINING:     Serial.println("EV_JOINING"); break;
        case EV_JOINED:      Serial.println("EV_JOINED"); LMIC_setLinkCheckMode(0); break;
        case EV_JOIN_FAILED: Serial.println("EV_JOIN_FAILED"); break;
        case EV_TXCOMPLETE:
            Serial.println("EV_TXCOMPLETE");
            if (LMIC.txrxFlags & TXRX_ACK) Serial.println("Received ACK");
            if (LMIC.dataLen) {
                Serial.print("Received ");
                Serial.print(LMIC.dataLen);
                Serial.println(" bytes of payload");
            }
            os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(LORAWAN_TX_INTERVAL), do_send);
            break;
        default:
            Serial.print("Unknown event: ");
            Serial.println(ev);
            break;
    }
}

/**
 * do_send() is a callback function that is scheduled to run after a 
 * fixed interval (30 seconds) following the completion of a transmission.
 *  It checks if the LoRaWAN stack is currently busy with an ongoing transmission 
 * (indicated by the OP_TXRXPEND flag). If the stack is busy, it logs a message and 
 * returns without sending. If the stack is ready, it queues a placeholder payload 
 * for transmission. The actual telemetry data is sent via the sendTelemetry() function, 
 * which encodes the JSON document into a LoRaWAN payload and queues it for transmission.
 * This function ensures that transmissions are spaced out according to the defined interval 
 * and that the LoRaWAN stack is not overwhelmed with back-to-back transmissions.   
 * 
 */

static void do_send(osjob_t* j) {
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println("LoRaWAN: OP_TXRXPEND, not sending");
        return;
    }

    // Placeholder payload; real data comes via sendTelemetry()
    uint8_t payload[1] = { 0x00 };
    LMIC_setTxData2(1, payload, sizeof(payload), 0);
    Serial.println("LoRaWAN: uplink queued");
}

/**
 *  LoRaWANComms class methods  
 * - begin(): Initializes the LoRaWAN stack and starts the joining process. It also schedules the first uplink transmission.
 * - loop(): Should be called in the main loop to allow the LMIC library to process events and manage the LoRaWAN state machine.
 * - sendTelemetry(): Encodes a JSON document into a LoRaWAN payload and queues it for transmission. It checks if the LoRaWAN 
 *     stack is ready before sending and logs the payload size.
 * - publish(): For this implementation, it simply calls sendTelemetry() since LoRaWAN does not have a concept of 
 *     topics like MQTT. In a real application, you might want to include topic information in the payload or manage it in a 
 *     different way depending on your backend requirements. 
 * */   
void LoRaWANComms::begin() {
    Serial.println("LoRaWAN: begin()");
    os_init();
    LMIC_reset();
    LMIC_setDrTxpow(DR_SF7, 14);
    do_send(&sendjob);
}

void LoRaWANComms::loop() {
    os_runloop_once();
}

/**
 * sendTelemetry() encodes a JSON document into a LoRaWAN payload and queues it for transmission.
 * It first checks if the LoRaWAN stack is currently busy with an ongoing transmission 
 * (indicated by the OP_TXRXPEND flag). If the stack is busy, it logs a message and 
 * returns without sending. If the stack is ready, it encodes the JSON document into a LoRaWAN payload using the
 * encodeLoRaPayload() function (which should be defined in the LoRaWANEncoder module) and queues it for transmission 
 * using LMIC_setTxData2(). The function also logs the size of the payload being sent. This method allows the application 
 * to send telemetry data as JSON, which is then encoded into a format suitable for LoRaWAN transmission.
 * Note: Ensure that the encoded payload does not exceed the maximum payload size for the selected data rate, and consider 
 * implementing fragmentation if necessary for larger payloads. Additionally, be mindful of the duty cycle limitations of 
 * LoRaWAN when scheduling transmissions.
 *  
 */
void LoRaWANComms::sendTelemetry(const JsonDocument& doc) {
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println("LoRaWAN: busy, skip telemetry");
        return;
    }

    LoRaPayload p = encodeLoRaPayload(doc);
    LMIC_setTxData2(1, p.bytes, p.length, 0);
    Serial.print("LoRaWAN: telemetry queued, bytes=");
    Serial.println(p.length);
}

/**
 * publish() is a method that, in the context of LoRaWAN, simply calls sendTelemetry() 
 * since LoRaWAN does not have a concept of topics like MQTT. In a real application, you 
 * might want to include topic information in the payload or manage it in a different way
 *  depending on your backend requirements. For this implementation, publish() serves as an 
 * alias for sendTelemetry(), allowing the main application code to call publish() without 
 * needing to know the specifics of how telemetry data is sent over LoRaWAN.
 * 
 *  This design keeps the main application code clean and abstracts away the details of the
 *  communication protocol.   
 * 
 * Note: If you want to include topic information in the payload, you could modify 
 * the sendTelemetry() method to accept a topic parameter and encode it into the payload 
 * along with the JSON data. However, this would require a custom encoding scheme and 
 * careful consideration of payload size limitations.  
 * 
 */
void LoRaWANComms::publish(const char* topic, const JsonDocument& doc)
 { // send LoRaWAN uplink    
    sendTelemetry(doc);
 }

 
