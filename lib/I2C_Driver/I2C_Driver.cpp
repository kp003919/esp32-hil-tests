#include "I2C_Driver.h"
#include "driver/i2c.h"
#include "esp_log.h"

#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_PORT I2C_NUM_0
#define I2C_FREQ 100000

static const char *TAG = "I2C_DRIVER";

// Default mode = FAKE
data_mode_t currentMode =  MODE_FAKE;

// -------------------- INIT --------------------
void i2c_begin() {
    i2c_config_t config = {};
    config.mode = I2C_MODE_MASTER;
    config.sda_io_num = I2C_SDA;
    config.scl_io_num = I2C_SCL;
    config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    config.master.clk_speed = I2C_FREQ;

    i2c_param_config(I2C_PORT, &config);
    i2c_driver_install(I2C_PORT, config.mode, 0, 0, 0);
    //ESP_LOGI(TAG, "I2C initialized (C++ class)");
}

// -------------------- FAKE DATA --------------------
int32_t getFakeValue() {
    static int32_t fake = 1000;
    fake += 123;
    if (fake > 5000) fake = 1000;
    return fake;
}

// -------------------- HARDWARE DATA --------------------
int32_t getHardwareValue() {
    // TODO: Add real I2C sensor read later
    return 0;
}

// -------------------- PUBLIC API --------------------
int32_t getValue() {
    return (currentMode == MODE_FAKE)
        ? getFakeValue()
        : getHardwareValue();
}

void setMode(data_mode_t mode) {
    currentMode = mode;
}
