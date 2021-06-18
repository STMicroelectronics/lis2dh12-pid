#ifndef I2C_DRIVER
#define I2C_DRIVER

extern "C" {

/*!
 *  @brief Function for open I2C device.
 *
 *  @param[in] device : Path to charactere device.
 *
 *  @return Integer representing the file descriptor
 *  @retval >=0 -> Success
 *  @retval <0 -> Failure Info
 *
 */
int i2cOpen(const char *device);

/*!
 *  @brief Function for close I2C device.
 *
 *  @param[in] fd : File descriptor.
 *
 *  @return Status of close
 *  @retval =0 -> Success
 *  @retval <0 -> Failure Info
 *
 */
int i2cClose(int fd);

/*!
 *  @brief Function for writing the sensor's registers through I2C device.
 *
 *  @param[in] fd : File descriptor.
 *  @param[in] i2cAddr : Sensor I2C address.
 *  @param[in] regAddr : Register address.
 *  @param[in] regData : Pointer to the data buffer whose value is to be written.
 *  @param[in] length   : No of bytes to write.
 *
 *  @return Number of writed bytes
 *  @retval >0 -> Success
 *  @retval -1 -> Failure Info
 *
 */
int
i2cWrite(int fd, unsigned char i2cAddr, unsigned char regAddr, unsigned char *regData, int length);

/*!
 *  @brief Function for reading the sensor's registers through I2C device.
 *
 *  @param[in] fd : File descriptor.
 *  @param[in] i2Addr : Sensor I2C address.
 *  @param[in] regAddr : Register address.
 *  @param[out] regData    : Pointer to the data buffer to store the read data.
 *  @param[in] length   : No of bytes to read.
 *
 *  @return Number of readed bytes
 *  @retval >0 -> Success
 *  @retval -1 -> Failure Info
 *
 */
int
i2cRead(int fd, unsigned char i2cAddr, unsigned char regAddr, unsigned char *regData, int length);

};


class I2C {
public:
    I2C(const char* device_path, int i2c_addr) {
        device = i2cOpen(device_path);
        i2cAddr = i2c_addr;
    }
    I2C(int device_fd, int i2c_addr) {
        device = device_fd;
        i2cAddr = i2c_addr;
    }

    bool write(unsigned char regAddr, unsigned char *regData, int length) {
        return i2cWrite(device, i2cAddr, regAddr, regData, length) == length;
    }

    bool writeByte(unsigned char regAddr, unsigned char regData) {
        return i2cWrite(device, i2cAddr, regAddr, &regData, 1) == 1;
    }

    bool read(unsigned char regAddr, unsigned char *regData, int length) {
        return i2cRead(device, i2cAddr, regAddr, regData, length) == length;
    }

    bool readByte(unsigned char regAddr, unsigned char &regData) {
        return i2cRead(device, i2cAddr, regAddr, &regData, 1) == 1;
    }

    bool readShort(unsigned char regAddr, unsigned short &regData, bool lsb_first = true) {
        unsigned char tmp[2];
        bool ret = i2cRead(device, i2cAddr, regAddr, tmp, 2) == 2;
        if (ret) {
            unsigned short lsb, msb;
            if (lsb_first) {
                lsb = tmp[0];
                msb = tmp[1];
            } else {
                msb = tmp[0];
                lsb = tmp[1];
            }
            regData = (msb << 8) + lsb;
        }
        return ret;
    }

    bool isOpen() {
        return device > 0;
    }

    int getFileDescriptor(){
        return device;
    }
private:
    int device;
    int i2cAddr;
};

#endif  // I2C_DRIVER