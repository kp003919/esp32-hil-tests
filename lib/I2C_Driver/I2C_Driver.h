#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include <stdint.h>

typedef enum {
    MODE_FAKE,
    MODE_HARDWARE
} data_mode_t;


void i2c_init();
int32_t getFakeValue();
int32_t getHardwareValue();
void i2c_set_mode(data_mode_t mode);
void i2c_begin();

#endif
