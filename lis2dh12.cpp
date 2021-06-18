#include "lis2dh12.hpp"
#include "i2c_driver.h"
#include "spi_driver.h"

#include <fcntl.h>      // open
#include <unistd.h>     // usleep, write, read, close

/* Public methods */

LIS2DH12::LIS2DH12(const char *pathDevice, PowerMode powerMode, ConnectionMode conn){
    int filedescriptor;
    stmdev_ctx_t ctx;

    switch(conn) {
        case ConnectionMode.I2C:
            ctx.write_reg = i2c_write;
            ctx.read_reg = i2c_read;
            filedescriptor = i2cOpen(pathDevice);
            break;
        case ConnectionMode.SPI:
            ctx.write_reg = spi_write;
            ctx.read_reg = spi_read;
            filedescriptor = spiOpen(pathDevice);
            spiMaxSpeed = 100000; // study other best number
            spiMode = 0; // study other best number
            break;
    }

    if (filedescriptor > 0) {
        ctx.handle = (void*)&filedescriptor;
        ctxDevice = &ctx;
        configPowerMode(powerMode);
    } else {
        // TODO: check and alert error
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

char LIS2DH12::getInterrupt_1(){return -1;} // TODO

char LIS2DH12::getInterrupt_2(){return -1;} // TODO

int LIS2DH12::configPowerMode(PowerMode powerMode){return -1;} // TODO

int LIS2DH12::configInterruption(InterruptMode interruptMode){return -1;} // TODO

/* Private methods */

int LIS2DH12::initDevice(const char *pathDevice){
    return open(pathDevice, O_RDWR);
}

int32_t LIS2DH12::i2c_write(void *handle, uint8_t reg, const uint8_t *data, uint16_t len){
    int ret = i2cWrite((int)*handle, LIS2DH12_ID, reg, data, len);
    return (ret > 0) 0 : ret;
}

int32_t LIS2DH12::i2c_read(void *handle, uint8_t reg, uint8_t *data, uint16_t len){
    int ret = i2cRead((int)*handle, LIS2DH12_ID, reg, data, len);
    return (ret > 0) 0 : ret;
}

int32_t LIS2DH12::spi_write(void *handle, uint8_t reg, const uint8_t *data, uint16_t len){
    uint8_t config = 0x02; // write mode (bit 0 = 0); auto incremented address (bit 1 = 1)
    config |= (reg << 2);
    len += 1;
    uint8_t rx[len];
    uint8_t tx[len];
    tx[0] = config;
    tx+1 = data;

    return spiTransfer((int)*handle, tx, rx, len, spiMaxSpeed, spiMode);
}

int32_t LIS2DH12::spi_read(void *handle, uint8_t reg, uint8_t *data, uint16_t len){
    uint8_t config = 0x03; // read mode (bit 0 = 1); auto incremented address (bit 1 = 1)
    config |= (reg << 2);
    len += 1;
    uint8_t rx[len];
    uint8_t tx[len];
    tx[0] = config;

    // TODO: test if len should be incremented or not, and if the received data starts at rx[0] or rx[1]
    int ret spiTransfer((int)*handle, tx, rx, len, spiMaxSpeed, spiMode);
    data = rx
    return ret;
}
