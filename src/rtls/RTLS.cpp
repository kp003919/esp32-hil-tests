/**
 * RTLS (Real-Time Location System) module for ESP32    
 * - Scans for BLE beacons and extracts their information (ID, RSSI, battery, type)
 * - Uses an Exponential Moving Average (EMA) to smooth RSSI values for more stable distance estimation
 * - Stores up to 20 beacons in memory and updates their information on each scan   
 * Author: Muhsin Atto 
 * Date: 2024-06-01 
 * Details:
 * - The beacon payload is expected to be in a specific format (5 bytes of manufacturer data    
 *  - Byte 0: 0xFF (Manufacturer ID LSB)    
 * - Byte 1: 0xFF (Manufacturer ID MSB) 
 * - Byte 2: Beacon ID (0–255)
 * - Byte 3: Battery level (0–100)  
 * - Byte 4: Beacon type (0–255)
 * - The RTLS module provides a function to fill a JSON document with the current beacon information    
 * - The BLE scanning is performed periodically (every 2 seconds) to keep the beacon information updated
 * - The RSSI smoothing helps to mitigate fluctuations in signal strength for better distance estimation    
 *   
 */
#include "RTLS.h"
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// =========================
// Beacon Payload Format
// =========================
// Byte 0: 0xFF (Manufacturer ID LSB)
// Byte 1: 0xFF (Manufacturer ID MSB)
// Byte 2: Beacon ID
// Byte 3: Battery level (0–100)
// Byte 4: Beacon type

// =========================
// Beacon Structure
// =========================
struct BeaconInfo {
    uint8_t id;
    float rssiSmoothed;
    uint8_t battery;
    uint8_t type;
    uint32_t lastSeen;
    bool hasRssi = false;
};

static BeaconInfo beacons[20];
static uint8_t beaconCount = 0;

// BLE Scan object
static BLEScan* pBLEScan = nullptr;

// =========================
// RSSI Smoothing (EMA)
// =========================
static const float EMA_ALPHA = 0.30f;

/**
 * Update RSSI value using Exponential Moving Average (EMA) 
 * This function takes a BeaconInfo object and a new RSSI reading, and updates the 
 * smoothed RSSI value using the EMA formula. 
 * If the beacon does not have a previous RSSI value, it initializes it with the new    
 * reading. Otherwise, it applies the EMA formula to smooth out fluctuations in the RSSI 
 * readings over time. 
 * This helps to provide a more stable RSSI value for distance estimation,
 *  as raw RSSI readings can be noisy and fluctuate due to environmental factors.  
 *   
 * 
 */

void updateRSSI(BeaconInfo& b, int newRssi) {
    if (!b.hasRssi) {
        b.rssiSmoothed = newRssi;
        b.hasRssi = true;
    } else {
        b.rssiSmoothed = EMA_ALPHA * newRssi + (1.0f - EMA_ALPHA) * b.rssiSmoothed;
    }
}

/**
 * BLE Advertised Device Callback
 * This class implements the BLEAdvertisedDeviceCallbacks interface to handle BLE scan results.
 * When a new BLE device is detected during scanning, the onResult() method is called with the
 * advertised device information.   
 * The callback checks if the device has manufacturer data and if it matches the expected format for our beacons.
 * If it does, it extracts the beacon information (ID, battery, type) and updates the existing beacon entry or adds a new one to the list of detected beacons.
 * The RSSI value is also updated using the updateRSSI() function to apply smoothing.
 * This allows us to maintain an up-to-date list of nearby beacons with their information for use in the RTLS system.   
 *  
 */
class RTLSCallback : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice device) override {

        if (!device.haveManufacturerData()) return;
        std::string mfg = device.getManufacturerData();
        if (mfg.length() < 5) return;

        // Check manufacturer ID
        if ((uint8_t)mfg[0] != 0xFF || (uint8_t)mfg[1] != 0xFF)
            return;
        
        // Extract beacon information from manufacturer data    
        uint8_t id      = (uint8_t)mfg[2];
        uint8_t battery = (uint8_t)mfg[3];
        uint8_t type    = (uint8_t)mfg[4];
        int rssi        = device.getRSSI();

        // Update existing beacon
        for (uint8_t i = 0; i < beaconCount; i++) {
            if (beacons[i].id == id) {
                updateRSSI(beacons[i], rssi);
                beacons[i].battery  = battery;
                beacons[i].type     = type;
                beacons[i].lastSeen = millis();
                return;
            }
        }

        // Add new beacon
        if (beaconCount < 20) {
            beacons[beaconCount].id       = id;
            beacons[beaconCount].battery  = battery;
            beacons[beaconCount].type     = type;
            beacons[beaconCount].lastSeen = millis();
            updateRSSI(beacons[beaconCount], rssi);
            beaconCount++;
        }
    }
};

/**
 * RTLS Initialization
 * This function initializes the BLE scanning for the RTLS system. It sets up the BLE device    
 * and configures the BLE scan object with the appropriate parameters for active scanning. It also assigns the RTLSCallback to handle scan results.
 * The scan interval and window are set to values that allow for efficient scanning while balancing power consumption. 
 * This function should be called once during the setup phase of the program to start the BLE scanning process for detecting beacons in the environment.
 *  
 */
void RTLS_begin() {
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new RTLSCallback());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(80);
    pBLEScan->setWindow(79);
}

/**
 * RTLS Update
 * This function should be called periodically (e.g. in the main loop) to maintain the BLE scanning process. It checks if the specified time interval (2 seconds) has passed since the last scan, and if so, it starts a new BLE scan for a short duration (1 second) to update the list of detected beacons.
 * After each scan, it clears the previous scan results to prepare for the next round of scanning.
 * This allows the RTLS system to continuously update the information about nearby beacons, including their RSSI values, battery levels, and types, which can be used for real-time location tracking and other applications.
 *  
 */
void RTLS_update() {
    static uint32_t lastScan = 0;
    if (!pBLEScan) return;

    if (millis() - lastScan >= 2000) {  // scan every 2 seconds
        lastScan = millis();
        pBLEScan->start(1, false);      // 1 second scan
        pBLEScan->clearResults();
    }
}

/**
 * RTLS Fill JSON Document
 * This function fills a provided JSON document with the current information of detected beacons.
 *  It creates a JSON array under the "rtls" key and populates it with objects representing 
 * each beacon. Each beacon object contains the beacon ID, smoothed RSSI value, battery level, 
 * type, and the last seen timestamp.
 * This allows the RTLS system to provide a structured representation of the beacon information
 *  that can be easily serialized and sent to a backend or used for further processing in the 
 * application. The JSON document can then be used for telemetry, logging, or any other purpose 
 * where the beacon information is needed in a structured format.
 * 
 *  
 */
void RTLS_fill(JsonDocument& doc) {
    JsonArray arr = doc["rtls"].to<JsonArray>();
    for (uint8_t i = 0; i < beaconCount; i++) 
    {
        JsonObject b = arr.createNestedObject();
        b["id"]       = beacons[i].id;
        b["rssi"]     = (int)beacons[i].rssiSmoothed;
        b["battery"]  = beacons[i].battery;
        b["type"]     = beacons[i].type;
        b["lastSeen"] = beacons[i].lastSeen;
    }
}
