#ifndef I2C_DRIVER
#define I2C_DRIVER

extern "C" {

#include <cerrno>
#include <cstdint>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>  // ioctl
#include <unistd.h>     // usleep, write, read, close
#include <sys/stat.h>
#include <fcntl.h>      // open
#include <cstring>      // memset
#include <cstdio>

#define ERR_DEBUG 0
#define errDebug(value) if(ERR_DEBUG) perror(value)

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
int i2cOpen (const char *device) {
    return open (device, O_RDWR);
}

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
int i2cClose (int fd) {
    return close (fd);
}

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
i2cWrite (int fd, unsigned char i2cAddr, unsigned char regAddr, unsigned char *regData, int length) {
    if (fd < 0) {
        errDebug("Failed, invalid file descriptor");
        return -1;
    }
    ioctl (fd, I2C_SLAVE, i2cAddr);
    write (fd, &regAddr, 1);

    return write (fd, regData, length);
}

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
i2cRead (int fd, unsigned char i2cAddr, unsigned char regAddr, unsigned char *regData, int length) {
    if (fd < 0) {
        errDebug ("Failed, invalid file descriptor");
        return -1;
    }
    ioctl (fd, I2C_SLAVE, i2cAddr);
    write (fd, &regAddr, 1);

    return read (fd, regData, length);
}

};

#endif  // I2C_DRIVER