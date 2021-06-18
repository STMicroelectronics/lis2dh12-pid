#ifndef SPI_DRIVER
#define SPI_DRIVER

extern "C" {


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
int spiOpen(const char * device);

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
int spiClose(int fd);

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
int spiTransfer(int fd, unsigned char *tx, unsigned char *rx, int length, unsigned int maxSpeed, int mode);


};  // extern C

#endif  // SPI_DRIVER