/**
 * @file spi_driver.cpp
 * @brief SPI driver implementation for ESP32       
 * * This file provides a simple SPI driver implementation for the ESP32 microcontroller. It includes functions to initialize the SPI bus, transfer data, and read/write bytes. The driver uses the Arduino SPI library for underlying communication, making it easy to integrate with other components in the project that require SPI communication.
 * The SPI_Init function initializes the SPI bus, while SPI_Transfer allows for sending and receiving   
 * a single byte of data. The SPI_Write and SPI_Read functions handle writing and reading multiple bytes, respectively. This driver can be used to interface with various SPI peripherals such as sensors, displays, or memory devices in the IoT project.  
 *      
 */

#include "spi_driver.h"
#include <SPI.h>

// SPI driver implementation for ESP32
// This implementation uses the Arduino SPI library for underlying communication.   
void SPI_Init(void) {
    SPI.begin();
}

// Transfer a single byte over SPI and receive the response 
// This function sends a byte of data and returns the byte received from the SPI slave device. It can be used for simple command-response interactions with SPI peripherals.
// @param data The byte of data to send over SPI
// @return The byte received from the SPI slave device  

uint8_t SPI_Transfer(uint8_t data) {
    return SPI.transfer(data);
}

void SPI_Write(uint8_t *buffer, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        SPI_Transfer(buffer[i]);
    }
}

void SPI_Read(uint8_t *buffer, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        buffer[i] = SPI_Transfer(0xFF);
    }
}
