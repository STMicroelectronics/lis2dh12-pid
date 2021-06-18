#include "lis2dh12.hpp"

#include <fcntl.h>      // open
#include <unistd.h>     // usleep, write, read, close

/* Public methods */

LIS2DH12::LIS2DH12(const char *pathDevice, PowerMode powerMode, ConnectionMode conn){
    int fdDevice = initDevice(pathDevice);

    if (fdDevice > 0) {
        stmdev_ctx_t ctx;
        ctx.handle = (void*)&fdDevice;
        switch(conn) {
            case ConnectionMode.I2C:
            ctx.write_reg = i2c_write;
            ctx.read_reg = i2c_read;
            break;
            case ConnectionMode.SPI:
            ctx.write_reg = spi_write;
            ctx.read_reg = spi_read;
            break;
        }
        ctxDevice = &ctx;

        configPowerMode(powerMode);
    }
}


int LIS2DH12::getAccelerationRaw(int16_t *x, int16_t *y, int16_t *z){
    int16_t acc[3];
    int ret = lis2dh12_acceleration_raw_get(ctxDevice, acc);
    x = acc;
    y = acc + 1;
    z = acc + 2;
    return ret;
}

int LIS2DH12::getTemperatureRaw(int16_t *temp){
    int16_t tmp;
    int ret = lis2dh12_temperature_raw_get(ctxDevice, tmp);
    temp = &tmp;
    return ret;
}

// char LIS2DH12::getInterrupt_1(){

// }

// char LIS2DH12::getInterrupt_2(){

// }

int LIS2DH12::configPowerMode(PowerMode powerMode){

}

int LIS2DH12::configInterruption(InterruptMode interruptMode){

}

/* Private methods */

int LIS2DH12::initDevice(const char *pathDevice){
    return open(pathDevice, O_RDWR);
}

int32_t LIS2DH12::i2c_write(void *handle, uint8_t reg, const uint8_t *data, uint16_t len){

    return 0; /* no Error */
}

int32_t LIS2DH12::i2c_read(void *handle, uint8_t reg, uint8_t *data, uint16_t len){

    return 0; /* no Error */
}

int32_t LIS2DH12::spi_write(void *handle, uint8_t reg, const uint8_t *data, uint16_t len){

    return 0; /* no Error */
}

int32_t LIS2DH12::spi_read(void *handle, uint8_t reg, uint8_t *data, uint16_t len){

    return 0; /* no Error */
}
