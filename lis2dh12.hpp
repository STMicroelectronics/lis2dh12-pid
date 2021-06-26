#ifndef LIS2DH12_DRIVER
#define LIS2DH12_DRIVER

#include "lis2dh12_reg.h"
#include <unistd.h>         /* usleep */


class LIS2DH12 {
public:
    LIS2DH12(lis2dh12_op_md_t op, stmdev_write_ptr wr, stmdev_read_ptr rd);

    ~LIS2DH12(){}

    /**
     * Return the error code generated on execution of the last method of the object.
     * MANDATORY: return 0 -> no Error.
     * @return int Integer corresponding to error code.
     */
    int have_error() {
        return error_status;
    }
    
    void enable_sensor(lis2dh12_odr_t odr);
    void disable_sensor();
    void enable_temperature();
    void disable_temperature();
    int16_t* acceleration_raw();
    int16_t* acceleration();
    int16_t temperature_raw();
    int16_t temperature();
    void config_interruption(lis2dh12_ctrl_reg3_t, uint8_t, uint8_t);
    lis2dh12_int1_src_t interrupt_1();
    lis2dh12_int2_src_t interrupt_2();

private:
    void wait_value() {
        usleep(microsecond);
    }

    stmdev_dev_ctx_t dev_ctx[1];
    lis2dh12_op_md_t operation_mode;
    int error_status;
    unsigned int microsecond;
};

#endif // LIS2DH12_DRIVER
