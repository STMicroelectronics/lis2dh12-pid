#include "lis2dh12.hpp"
#include "i2c_driver.h"
#include "spi_driver.h"

#include <fcntl.h>      // open
#include <unistd.h>     // usleep, write, read, close

typedef float_t (*convert_int16_float_ptr) (int16_t);
convert_int16_float_ptr convert_acceleration;
convert_int16_float_ptr convert_temperature;


LIS2DH12::LIS2DH12(lis2dh12_op_md_t op, stmdev_write_ptr wr, stmdev_read_ptr rd) {
    // NOTE: if to use SPI, check CTRL_REG4 bit 0 and edit this code if to need
    dev_ctx->write_reg = wr;
    dev_ctx->read_reg = rd;
    
    operation_mode = op;
    error_status = 0;
    microsecond = 0;
    
    uint8_t whoamI
    lis2dh12_device_id_get(dev_ctx, &whoamI);
    if (whoamI != LIS2DH12_ID) {
        error_status = -1;
    } else {
        lis2dh12_operating_mode_set(dev_ctx, op)
        disable();
    }
}

void LIS2DH12::enable_sensor(lis2dh12_odr_t odr){
    error_status = lis2dh12_data_rate_set(dev_ctx, odr);
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

void LIS2DH12::disable_sensor(){
    error_status = lis2dh12_data_rate_set(dev_ctx, LIS2DH12_POWER_DOWN);
}

void LIS2DH12::enable_temperature(){
    error_status = lis2dh12_temperature_meas_set(dev_ctx, LIS2DH12_TEMP_ENABLE);
}

void LIS2DH12::disable_temperature(){
    error_status = lis2dh12_temperature_meas_set(dev_ctx, LIS2DH12_TEMP_DISABLE);
}

int16_t* LIS2DH12::acceleration_raw(){
    uint8_t acc_ready;
    error_status = lis2dh12_xl_data_ready_get(dev_ctx, acc_ready);

    if (error_status != 0 || !acc_ready) {
        wait_value();
    }

    int16_t acc_raw[3];
    error_status = lis2dh12_acceleration_raw_get(dev_ctx, acc_raw);
    return acc_raw;
}

int16_t* LIS2DH12::acceleration(){
    int16_t* acc_raw = acceleration_raw();
    lis2dh12_fs_t fs_scale;
    lis2dh12_full_scale_get(dev_ctx, &fs_scale);

    switch (operation_mode) {
        case LIS2DH12_HR_12bit:
            switch(fs_scale) {
                case LIS2DH12_2g:
                    convert_acceleration = lis2dh12_from_fs2_hr_to_mg;
                    break;

                case LIS2DH12_4g:
                    convert_acceleration = lis2dh12_from_fs4_hr_to_mg;
                    break;

                case LIS2DH12_8g:
                    convert_acceleration = lis2dh12_from_fs8_hr_to_mg;
                    break;

                case LIS2DH12_16g:
                    convert_acceleration = lis2dh12_from_fs16_hr_to_mg;
                    break;

                default:
                    return .0;
            }
            break;

        case LIS2DH12_NM_10bit:
            switch(fs_scale) {
                case LIS2DH12_2g:
                    convert_acceleration = lis2dh12_from_fs2_nm_to_mg;
                    break;

                case LIS2DH12_4g:
                    convert_acceleration = lis2dh12_from_fs4_nm_to_mg;
                    break;

                case LIS2DH12_8g:
                    convert_acceleration = lis2dh12_from_fs8_nm_to_mg;
                    break;

                case LIS2DH12_16g:
                    convert_acceleration = lis2dh12_from_fs16_nm_to_mg;
                    break;

                default:
                    return .0;
            }
            break;

        case LIS2DH12_LP_8bit:
            switch(fs_scale) {
                case LIS2DH12_2g:
                    convert_acceleration = lis2dh12_from_fs2_lp_to_mg;
                    break;

                case LIS2DH12_4g:
                    convert_acceleration = lis2dh12_from_fs4_lp_to_mg;
                    break;

                case LIS2DH12_8g:
                    convert_acceleration = lis2dh12_from_fs8_lp_to_mg;
                    break;

                case LIS2DH12_16g:
                    convert_acceleration = lis2dh12_from_fs16_lp_to_mg;
                    break;

                default:
                    return .0;
            }
            break;

        default:
            return .0;
    }

    float_t acc_f[3];
    acc_f[0] = convert_acceleration(acc_raw[0]);
    acc_f[1] = convert_acceleration(acc_raw[1]);
    acc_f[2] = convert_acceleration(acc_raw[2]);
    return acc_f;
}

int16_t LIS2DH12::temperature_raw(){
    uint8_t temp_ready;
    error_status = lis2dh12_temp_data_ready_get(dev_ctx, temp_ready);

    if (error_status != 0 || !temp_ready) {
        wait_value();
    }

    int16_t temp;
    error_status = lis2dh12_temperature_raw_get(dev_ctx, &temp);
    return temp;
}

int16_t LIS2DH12::temperature(){
    int16_t temp_raw = temperature_raw();

    switch (operation_mode) {
        case LIS2DH12_HR_12bit:
            convert_temperature = lis2dh12_from_lsb_hr_to_celsius;
            break;

        case LIS2DH12_NM_10bit:
            convert_temperature = lis2dh12_from_lsb_nm_to_celsius;
            break;

        case LIS2DH12_LP_8bit:
            convert_temperature = lis2dh12_from_lsb_lp_to_celsius;
            break;

        default:
            return 0;
    }

    return convert_temperature(temp_raw);
}

void LIS2DH12::config_interruption(lis2dh12_ctrl_reg3_t cfg, uint8_t threshold, uint8_t duration){
    error_status = lis2dh12_pin_int1_config_set(dev_ctx, cfg);
    if (error_status != 0) return;
    error_status = lis2dh12_int1_gen_threshold_set(dev_ctx, threshold);
    if (error_status != 0) return;
    error_status = lis2dh12_int1_gen_duration_set(dev_ctx, duration);
}

lis2dh12_int1_src_t LIS2DH12::interrupt_1(){
    lis2dh12_int1_src_t val;
    error_status = lis2dh12_int1_gen_source_get(dev_ctx, &val);
    return val;
}

lis2dh12_int2_src_t LIS2DH12::interrupt_2(){
    lis2dh12_int2_src_t val;
    error_status = lis2dh12_int2_gen_source_get(dev_ctx, &val);
    return val;
}
