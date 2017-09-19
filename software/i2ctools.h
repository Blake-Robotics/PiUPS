#ifndef PIUPS_I2C_TOOLS_H
#define PIUPS_I2C_TOOLS_H

#include <stdint.h>

/** \file
 *  \brief  PiUPS I2C tools
 *  Set of tools to make accessing registers on the PiUPS eaisier
 *  and more consistent, with some form of error trapping.
 */


/// Function to open the I2C device as a file:
/** Function to open the PiUPS I2C device as a file
    
    \param file_i2c pointer which will be written with a file handle
    \param filename filename of the i2c device to be opened
    \return zero on success
*/
int open_i2c_dev(int *file_i2c, char* filename, uint8_t i2c_addr);


/// Function to write to a register on the PiUPS using the I2C file:
/** Function to write to a register on the PiUPS using the I2C file
    
    \param file_i2c the i2c device file handle
    \param reg the register on the device to write to
    \param data data to write to the register
    \return zero on success
*/
int write_i2c_reg(int file_i2c, uint8_t reg, uint16_t data);


/// Funtion to read from a register on the PiUPS using I2C.
/** Funtion to read from a register on the PiUPS using I2C.
    \param file_i2c the i2c device file handle
    \param reg the register on the device to read from
    \param data pointer to the memory location to store the data returned
    \return zero on success
*/
int read_i2c_reg(int file_i2c, uint8_t reg, uint16_t* data);

#endif // PIUPS_I2C_TOOLS_H



