#ifndef SPI_DRIVER
#define SPI_DRIVER

extern "C" {

#include <cerrno>
#include <cstdint>
#include <cstring>      // memset
#include <unistd.h>		// close
#include <cstdio>		// perror
#include <cstdlib>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>

#include <linux/spi/spidev.h>

#define ERR_DEBUG 0
#define errDebug(value) if(ERR_DEBUG) perror(value)

/*!
 *  @brief Function for open SPI device.
 *
 *  @param[in] device : Path to charactere device.
 *
 *  @return Integer representing the file descriptor
 *  @retval >=0 -> Success
 *  @retval <0 -> Failure Info
 *
 */
int spiOpen(const char * device) {
    return open(device, O_RDWR);
}

/*!
 *  @brief Function for close SPI device.
 *
 *  @param[in] fd: File descriptor.
 *
 *  @return Status of close
 *  @retval =0 -> Success
 *  @retval <0 -> Failure Info
 *
 */
int spiClose(int fd) {
    return close(fd);
}

/*!
 *  @brief Function for transfer data to SPI device.
 *
 *  @param[in] device : Path to charactere device.
 *  @param[in] tx : Array to send data.
 *  @param[in] rx : Array to receiver data.
 *  @param[in] length : length of tx and rx to transfer.
 *  @param[in] max_speed : Max speed operation (Hz).
 *  @param[in] mode : Mode of operation: SPI_MODE_0, SPI_MODE_1, SPI_MODE_2, SPI_MODE_3.
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval <0 -> Failure Info
 *
 */
int spiTransfer(int fd, unsigned char *tx, unsigned char *rx, int length, unsigned int maxSpeed, int mode) {
    int ret;
    unsigned char bits = 8;

    if (fd < 0) {
        errDebug("Failed, invalid file descriptor");
        return fd;
    }

    ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
    if (ret == -1) {
        errDebug("Failed to set mode");
        return ret;
    }

    ret = ioctl(fd, SPI_IOC_RD_MODE32, &mode);
    if (ret == -1) {
        errDebug("Failed to set mode");
        return ret;
    }

    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1) {
        errDebug("Failed to set bits/word");
        return ret;
    }

    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1) {
        errDebug("Failed to set bits/word");
        return ret;
    }

    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &maxSpeed);
    if (ret == -1) {
        errDebug("Failed to set max speed (Hz)");
        return ret;
    }

    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &maxSpeed);
    if (ret == -1) {
        errDebug("Failed to set max speed (Hz)");
        return ret;
    }

    struct spi_ioc_transfer tr = {
            .tx_buf = (unsigned long)tx,
            .rx_buf = (unsigned long)rx,
            .len = (__u32)length,
            .speed_hz = maxSpeed,
            .bits_per_word = bits,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 0) {
        errDebug("Failed to send message");
    }

    return ret;
}

};

#endif  // SPI_DRIVER