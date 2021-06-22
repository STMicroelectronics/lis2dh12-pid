#include "lis2dh12.hpp"
#include "i2c_driver.h"
#include "spi_driver.h"

#include <fcntl.h>      // open
#include <unistd.h>     // usleep, write, read, close


void LIS2DH12::enable(lis2dh12_odr_t odr){
    error_status = lis2dh12_data_rate_set(ctx, odr);
    switch (odr) {
    case LIS2DH12_POWER_DOWN:
        microsecond = 0;
        break;
    case LIS2DH12_ODR_1Hz:
        microsecond = 1000000;  // (1/1Hz)   = 1s
        break;
    case LIS2DH12_ODR_10Hz:
        microsecond = 100000;   // (1/10Hz)  = 0.1s
        break;
    case LIS2DH12_ODR_25Hz:
        microsecond = 40000;    // (1/25Hz)  = 0.04s
        break;
    case LIS2DH12_ODR_50Hz:
        microsecond = 20000;    // (1/50Hz)  = 0.02s
        break;
    case LIS2DH12_ODR_100Hz:
        microsecond = 10000;    // (1/100Hz) = 0.001s
        break;
    case LIS2DH12_ODR_200Hz:
        microsecond = 5000;     // (1/200Hz) = 0.0005s
        break;
    case LIS2DH12_ODR_400Hz:
        microsecond = 2500;     // (1/400Hz) = 0.00025s
        break;
    case LIS2DH12_ODR_1kHz620_LP:
        microsecond = 618;      // (1/1620Hz) = 617.28 us
        break;
    case LIS2DH12_ODR_5kHz376_LP_1kHz344_NM_HP:
        if (operation_mode != LIS2DH12_LP_8bit)
            microsecond = 745;  // (1/1344Hz) = 744.05 us
        else
            microsecond = 187;  // (1/5376Hz) = 186.01 us
        break;
    default:
        microsecond = 0;
        break;
    }
}

void LIS2DH12::disable(){
    error_status = lis2dh12_data_rate_set(ctx, LIS2DH12_POWER_DOWN);
}

int16_t* LIS2DH12::acceleration_raw(){
    uint8_t acc_ready;
    error_status = lis2dh12_xl_data_ready_get(ctx, acc_ready);

    if (error_status != 0 || !acc_ready) {
        wait_value();
    }

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
    uint8_t temp_ready;
    error_status = lis2dh12_xl_data_ready_get(ctx, temp_ready);

    if (error_status != 0 || !temp_ready) {
        wait_value();
    }

    int16_t temp;
    error_status = lis2dh12_temperature_raw_get(ctx, &temp);
    return temp;
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
