#ifndef LIS2DH12_DRIVER
#define LIS2DH12_DRIVER

#include "lis2dh12_reg.h"

enum PowerMode {
    down, low, normal, high
};

enum InterruptMode {
    OR_COMBINATION        = 0X00,
    MOVEMENT_RECOGNITION  = 0X40,
    AND_COMBINATION       = 0x80,
    POSTITION_RECOGNITION = 0xC0
};

enum ConnectionMode {
    I2C, SPI
};

class LIS2DH12 {
public:
    LIS2DH12(const char*, PowerMode, ConnectionMode);
    ~LIS2DH12() {
        close(getfd());
    }
    
    bool isOpen(){
        return getfd() > 0;
    }
    int getAccelerationRaw(int16_t *x, int16_t *y, int16_t *z);
    int getTemperatureRaw(int16_t *temp);
    char getInterrupt_1();
    char getInterrupt_2();
    int configPowerMode(PowerMode);
    int configInterruption(InterruptMode);

private:
    stmdev_ctx_t *ctxDevice;
    
    int getfd(){
        if (ctxDevice != NULL && ctxDevice->handle != NULL) {
            return (int)*(ctxDevice->handle);
        }
        return -1;
    }
    int initDevice(const char*);

    int32_t i2c_write(void *handle, uint8_t reg, const uint8_t *data, uint16_t len);
    int32_t i2c_read (void *handle, uint8_t reg, uint8_t *data, uint16_t len);
    int32_t spi_write(void *handle, uint8_t reg, const uint8_t *data, uint16_t len);
    int32_t spi_read (void *handle, uint8_t reg, uint8_t *data, uint16_t len);

    unsigned int spiMaxSpeed = 0;
    int spiMode = 0;
};

#endif // LIS2DH12_DRIVER