/* I2C tools for easy access to the PiUPS registers
 * over i2c.
 *
 */


#include <unistd.h>			//Needed for I2C port
#include <fcntl.h>			//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
#include <stdint.h>
#include <stdio.h>
#include "i2ctools.h"


int open_i2c_dev(int *file_i2c, char* filename, uint8_t i2c_addr)
{
  // open the file:
  if ((*file_i2c = open(filename, O_RDWR)) < 0)
  {
    printf("Failed to open the i2c bus\n");
    return -1;
  }
 
  // select the slave address:
  if (ioctl(*file_i2c, I2C_SLAVE, i2c_addr) < 0)
  {
    printf("Failed to acquire bus access and/or talk to slave.\n");
    return -2;
  }
  return 0;
}



//Write to a specific I2C register:
int write_i2c_reg(int file_i2c, uint8_t reg, uint16_t data)
{
  // Setup data to write:
  uint8_t writebuffer[3];
  writebuffer[0] = reg;
  writebuffer[1] = (data >> 8);
  writebuffer[2] = data;
  
  // Write the data, and check that the bytes were really written
  if (write(file_i2c, writebuffer, 3) != 3)
  {
    /* ERROR HANDLING: i2c transaction failed */
    printf("Failed to write to the i2c bus.\n");
    return -1;
  }
  else
  {
    return 0;
  }
}


int read_i2c_reg(int file_i2c, uint8_t reg, uint16_t* data)
{
  // Read buffer to read data into:
  uint8_t readbuffer[2];
  
  // Write the data, and check that the bytes were really written
  if (write(file_i2c, &reg, 1) != 1)
  {
    /* ERROR HANDLING: i2c transaction failed */
    printf("Failed to write to the i2c bus.\n");
    return -1;
  }
  else
  {
    // Sleep and do a read of the register:
    usleep (1000);

    if (read(file_i2c, readbuffer, 2) != 2)
    {
      //ERROR HANDLING: i2c transaction failed
      printf("Failed to read from the i1c bus.\n");
      return -2;
    }
    else
    {
      *data = ((uint16_t)readbuffer[0] << 8) + readbuffer[1];
      return 0;
    }
  }
}
