#include "PiUPStools.h"
#include <stdio.h> 
#include <stdint.h>
#include <unistd.h>
#include "PiUPSDefines.h"

void piups_printstats(int file_i2c)
{
  uint16_t vcc, vbat, vrail, vaux1, vaux2, status; 
  
  if (read_i2c_reg(file_i2c, PIUPS_STATUS, &status) == 0)
  {
    printf("Read Status as as 0x%x. \n", vcc);
  }
  else
  {
    printf("Failed to read Status \n");
  }

  usleep (1000);

  if (read_i2c_reg(file_i2c, PIUPS_VCC, &vcc) == 0)
  {
    printf("Read VCC as %d mV. \n", vcc);
  }
  else
  {
    printf("Failed to read VCC \n");
  }

  usleep (1000);
	
  if (read_i2c_reg(file_i2c, PIUPS_VBATT, &vbat) == 0)
  {
    printf("Read VCC as %d mV. \n", vbat);
  }
  else
  {
    printf("Failed to read VBatt \n");
  }

  usleep (1000);	

  if (read_i2c_reg(file_i2c, PIUPS_VRAIL, &vrail) == 0)
  {
    printf("Read VRail as %d mV. \n", vrail);
  }
  else
  { 
    printf("Failed to read VRail \n");
  }

  usleep (1000);	
  
  if (read_i2c_reg(file_i2c, PIUPS_VAUX1, &vaux1) == 0)
  {
    printf("Read VAUX1 as %d mV. \n", vaux1);
  }
  else
  { 
    printf("Failed to read VAUX1 \n");
  }
  
  usleep (1000);	
  
  if (read_i2c_reg(file_i2c, PIUPS_VAUX2, &vaux2) == 0)
  {
    printf("Read VAUX2 as %d mV. \n", vaux2);
  }
  else
  { 
    printf("Failed to read VAUX2 \n");
  }

  return 0;
}
