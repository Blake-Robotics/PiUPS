#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
#include <stdint.h>
#include <stdio.h>

#include "i2ctools.h"
#include "PiUPSDefines.h"

int main (void)
{
  int file_i2c;
  char *filename = (char*)"/dev/i2c-1";

  // Parameters:
  uint16_t vcc;
  uint16_t vbat; 

  // Open the I2C device:
  if (open_i2c_dev(&file_i2c, filename, 0x13) != 0)
  {
    printf("Failed to setup i2c");
    return -1;
  }
  	
  if (read_i2c_reg(file_i2c, PIUPS_VCC, &vcc) == 0)
  {
    printf("Read VCC as %d mV. \n", vcc);
  }
  else
  {
    printf("Failed to read VCC \n");
  }
	
  if (read_i2c_reg(file_i2c, PIUPS_VBATT, &vbat) == 0)
  {
    printf("Read VCC as %d mV. \n", vbat);
  }

  usleep (1000);	
	
  // Write and Read Vbat conv:
  uint16_t vbat_conv_set = 4880, vbat_conv_read;
  if (write_i2c_reg(file_i2c, 0x50, vbat_conv_set) == 0)
  {
    printf("Set vbat_conv to %i. \n", vbat_conv_set);
  }
 
  usleep (1000);
  if (read_i2c_reg(file_i2c, 0x50, &vbat_conv_read) == 0)
  {
    printf("Read vbat_conv as %i. \n", vbat_conv_read);
  }
  
  return 0;
}
