#include "lis2dh12.hpp"
#include "i2c_driver.h"
#include "spi_driver.h"

#include <fcntl.h>      // open
#include <unistd.h>     // usleep, write, read, close


void LIS2DH12::enable(lis2dh12_odr_t odr){
    error_status = lis2dh12_data_rate_set(ctx, odr);
}

void LIS2DH12::disable(){
    error_status = lis2dh12_data_rate_set(ctx, LIS2DH12_POWER_DOWN);
}

int16_t* LIS2DH12::acceleration_raw(){
    int16_t acc[3];
    error_status = lis2dh12_acceleration_raw_get(ctx, acc);
    return acc;
}

int16_t LIS2DH12::x_axis(){
    int16_t *acc = acceleration_raw();
    return acc[0];
}

int16_t LIS2DH12::y_axis(){
    int16_t *acc = acceleration_raw();
    return acc[1];
}

int16_t LIS2DH12::z_axis(){
    int16_t *acc = acceleration_raw();
    return acc[2];
}

int16_t LIS2DH12::temperature_raw(){
    int16_t tmp;
    error_status = lis2dh12_temperature_raw_get(ctx, &tmp);
    return tmp;
}

void LIS2DH12::config_interruption(lis2dh12_ctrl_reg3_t val){
    error_status = lis2dh12_pin_int1_config_set(ctx, val);
}

lis2dh12_int1_src_t LIS2DH12::interrupt_1(){
    lis2dh12_int1_src_t val;
    error_status = lis2dh12_int1_gen_source_get(ctx, &val);
    return val;
}

lis2dh12_int2_src_t LIS2DH12::interrupt_2(){
    lis2dh12_int2_src_t val;
    error_status = lis2dh12_int2_gen_source_get(ctx, &val);
    return val;
}
