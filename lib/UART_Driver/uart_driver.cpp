#include "uart_driver.h"
#include <Arduino.h>

static bool stm32_uart_enabled = true;
static char stm32_last_packet[64] = {0};


void UART2_Init(void) {
    Serial2.begin(115200, SERIAL_8N1, 16, 17);  
    // RX = GPIO16, TX = GPIO17
}

void UART2_Enable(bool state) {
    stm32_uart_enabled = state;
}

const char* UART2_GetPacket(void) {
    return stm32_last_packet;
}


void UART_Init(void) {
    // Start UART at 115200 baud (change if needed)
    Serial.begin(115200);

    // Wait for serial port to be ready (optional)
    while (!Serial) {
        ; // Wait
    }
}

void UART_SendByte(uint8_t data) {
    Serial.write(data);
}

void UART_SendString(const char *str) {
    Serial.print(str);
}

uint8_t UART_ReadByte(void) {
    if (Serial.available() > 0) {
        return Serial.read();
    }
    return 0; // Default return if nothing available
}

bool UART_Available(void) {
    return Serial.available() > 0;
}

void UART2_Task(void) {
    if (!stm32_uart_enabled) return;

    while (Serial2.available()) {
        String packet = Serial2.readStringUntil('\n');
        packet.trim();

        if (packet.length() > 0) {
            strncpy(stm32_last_packet, packet.c_str(), sizeof(stm32_last_packet));
            stm32_last_packet[sizeof(stm32_last_packet)-1] = '\0';

            Serial.print("STM32 UART: ");
            Serial.println(stm32_last_packet);
        }
    }
}
