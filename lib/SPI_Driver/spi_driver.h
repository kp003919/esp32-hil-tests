#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

#include <stdint.h>

/**
 * @file spi_driver.h   
 * @brief SPI driver header for ESP32
 * This header file defines the interface for a simple SPI driver implementation for the ESP32 microcontroller  
 *
 */
void SPI_Init(void);
uint8_t SPI_Transfer(uint8_t data);
void SPI_Write(uint8_t *buffer, uint16_t length);
void SPI_Read(uint8_t *buffer, uint16_t length);

#endif
