/* PiUPS Status
 *
 * Application to return the status of the PiUPS microcontroller.
 *
 * 
 */

#include <unistd.h>                             //Needed for I2C port
#include <stdint.h>
#include <stdio.h>

#include "i2ctools.h"
#include "PiUPSDefines.h"
#include "PiUPStools.h"



int main(void)
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

  piups_printstats(file_i2c);

}

