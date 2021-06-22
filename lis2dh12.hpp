#ifndef LIS2DH12_DRIVER
#define LIS2DH12_DRIVER

#include "lis2dh12_reg.h"
#include <unistd.h>         /* usleep */


class LIS2DH12 {
public:
    LIS2DH12(lis2dh12_op_md_t op, stmdev_write_ptr wr, stmdev_read_ptr rd) {
        // NOTE: if to use SPI, check CTRL_REG4 bit 0 and edit this code if to need
        ctx->write_reg = wr;
        ctx->read_reg = rd;
        
        operation_mode = op;
        error_status = 0;
        microsecond = 0;
        
        uint8_t whoamI
        lis2dh12_device_id_get(&dev_ctx, &whoamI);
        if (whoamI != LIS2DH12_ID) {
            error_status = -1;
        } else {
            disable();
        }
    }

    ~LIS2DH12(){}

    int get_error() {
        return error_status;
    }
    
    void enable(lis2dh12_odr_t odr);
    void disable();
    int16_t* acceleration_raw();
    int16_t* acceleration();
    int16_t temperature_raw();
    int16_t temperature();
    void config_interruption(lis2dh12_ctrl_reg3_t);
    lis2dh12_int1_src_t interrupt_1();
    lis2dh12_int2_src_t interrupt_2();

private:
    void wait_value() {
        usleep(microsecond);
    }

    stmdev_ctx_t ctx[1];
    lis2dh12_op_md_t operation_mode;
    int error_status;
    unsigned int microsecond;
};

#endif // LIS2DH12_DRIVER
