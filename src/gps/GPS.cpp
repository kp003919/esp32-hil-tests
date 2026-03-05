/**
 * GPS Module
 * This module provides an interface for initializing, updating, and filling GPS data into a JSON 
 * document  
 * The GPS module uses the TinyGPS++ library to parse GPS data from a serial connection. 
 * It simulates GPS data for testing purposes, allowing the system to function even without a 
 * real GPS signal. The GPS_fill function populates a JSON document with the current GPS data,
 *  which can be sent as telemetry to an MQTT broker or used in other parts of the application. 
 * The module is designed to be easily integrated into the main program loop, where it can conti
 * nuously update and provide GPS information as needed.  
 * Note: The current implementation uses simulated GPS data for testing. In a real application, 
 * the GPS_update function would read from the serial connection and update the GPS data based 
 * on the actual GPS signal received.
 * Author: Muhsin 
 * Date: 2024-06
 * 
 */
#include "GPS.h"
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

// Define GPS mode: SIMULATED for testing without a real GPS signal, REAL for actual GPS hardware.  
// Set to GPS_MODE_REAL when using with actual GPS hardware.
#define GPS_MODE_SIMULATED 1  
#define GPS_MODE_REAL      2    

#define GPS_MODE GPS_MODE_SIMULATED // Change to GPS_MODE_REAL when using with actual GPS hardware  

static TinyGPSPlus gps;
static HardwareSerial SerialGPS(2);

// Simple simulated path
struct SimLocation {
    double lat, lon, alt, speed;
};



// Simulated path for testing GPS functionality without a real GPS signal   

static SimLocation path[] = {
    {43.213387, -80.986807, 80.0, 0.0},
    {53.3811,   -1.4701,   120.0, 10.0},
    {53.4808,   -2.2426,   100.0, 30.0},
    {53.8008,   -1.5491,   110.0, 25.0},
    {51.5074,   -0.1278,    50.0, 40.0}
};

static uint8_t currentPoint = 0;
static uint32_t lastMove = 0;
static const uint32_t moveInterval = 2000;

static double simLat = path[0].lat;
static double simLon = path[0].lon;
static double simAlt = path[0].alt;
static double simSpeed = path[0].speed;

/**
 * GPS Initialization   
 * This function initializes the GPS module by setting up the serial communication with
 *  the GPS hardware. It configures the serial port to use a baud rate of 9600 and sets 
 * the appropriate pins for RX and TX. This function should be called once during the setup 
 * phase of the program to prepare the GPS module for receiving data.
 */
void GPS_begin() {
    SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
}

/**
 * GPS Update
 * This function updates the GPS data by reading from the serial connection and parsing it 
 * using the TinyGPS++ library. It also simulates GPS movement along a predefined path for 
 * testing purposes. The function checks if there is new data available from the GPS module 
 * and updates the GPS object accordingly. Additionally, it simulates movement by gradually 
 * changing the simulated latitude, longitude, altitude, and speed towards the next point in 
 * the path every 2 seconds. This allows the system to function and provide GPS data even without
 *  a real GPS signal, which is useful for testing and development.
 * In a real application, the GPS_update function would primarily focus on reading and 
 * parsing data from the GPS hardware, while the simulation code can be removed or disabled
 *  when a real GPS signal is available.    
 * GPS MODE can be switched between SIMULATED and REAL to control whether the system uses 
 * simulated GPS data or real GPS data from the hardware.   
 */
void GPS_update() {
    while (SerialGPS.available() > 0) {
        gps.encode(SerialGPS.read());
    }
    // Simulate GPS movement for testing without a real GPS signal  
   if (GPS_MODE == GPS_MODE_SIMULATED) {    
     uint32_t now = millis();
     if (now - lastMove < moveInterval) return;

     SimLocation target = path[currentPoint];
     simLat   += (target.lat  - simLat)  * 0.05;
     simLon   += (target.lon  - simLon)  * 0.05;
     simAlt   += (target.alt  - simAlt)  * 0.05;
     simSpeed  = target.speed;

     if (fabs(simLat - target.lat) < 0.0001 &&
        fabs(simLon - target.lon) < 0.0001) {
        currentPoint = (currentPoint + 1) % (sizeof(path) / sizeof(path[0]));
     }

    lastMove = now;
}
}
/** 
* GPS Fill
* This function fills a JSON document with the current GPS data. It checks if the GPS location      
* data is valid and populates the JSON document with the latitude, longitude, altitude, and speed.
* If the GPS data is not valid, it sets the corresponding fields in the JSON document to null.
* For testing purposes, it can switch between simulated and real GPS data.
 In a real application, it would * use the actual GPS data obtained from the TinyGPS++ library. 
 This function allows the GPS data
* to be easily included in telemetry messages or other parts of the application that require GPS information.   
* The JSON document is expected to have "lat", "lon", "alt", and "speed" keys where the GPS data
* will be stored. This function should be called periodically (e.g., in the main loop) to keep the GPS data updated in the JSON document.
*  
 */
void GPS_fill(JsonDocument& doc) {
    // In a real application, this would use gps.location.lat(), gps.location.lng(), etc.   
    if (GPS_MODE == GPS_MODE_REAL) 
    {
        if (gps.location.isValid()) {
            Serial.println("GPS data valid, filling JSON document.");   
            Serial.print("Latitude: "); Serial.println(gps.location.lat(), 6);
            Serial.print("Longitude: "); Serial.println(gps.location.lng(), 6); 
            Serial.print("Altitude: "); Serial.println(gps.altitude.meters(), 2);
            Serial.print("Speed: "); Serial.println(gps.speed.kmph(), 2);   
            doc["lat"]   = gps.location.lat();  
            doc["lon"]   = gps.location.lng();
            doc["alt"]   = gps.altitude.meters();   
            doc["speed"] = gps.speed.kmph();
        } else {
            Serial.println("GPS data invalid, setting JSON fields to null.");   
            doc["lat"]   = nullptr; 
            doc["lon"]   = nullptr;
            doc["alt"]   = nullptr;     
            doc["speed"] = nullptr;
        }
    } else
       {   // Simulated GPS data for testing without a real GPS signal
           // Serial.println("Using simulated GPS data for testing.");          
            doc["lat"]   = simLat;
            doc["lon"]   = simLon;
            doc["alt"]   = simAlt;
            doc["speed"] = simSpeed;
        }
}