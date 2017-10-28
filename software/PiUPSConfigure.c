#include <unistd.h>                             //Needed for I2C port
#include <stdint.h>
#include <stdio.h>

#include "i2ctools.h"
#include "PiUPSDefines.h"
#include "PiUPStools.h"

int main(void)
{
  printf("PiUPSConfigure main:\n");

  // I2C interface setup:
  int file_i2c;
  const char *filename = (char*)"/dev/i2c-1";
  const uint8_t i2c_addr = 0x13;

  // Parameters to read
  uint16_t read_vbatt_conv, read_vrail_conv, read_vaux1_conv, read_vaux2_conv, read_v5v_conv;
  
  // Parameters to set, set != 0 to set the parameter.
  uint16_t set_vbatt_conv = 0; // VBatt Conversion - 4880
  uint16_t set_vrail_conv = 0; // VRail Conversion - 4351
  uint16_t set_vaux1_conv = 0; // VAUX1 Conversion - 11000
  uint16_t set_vaux2_conv = 0; // VAUX2 Conversion - 11000
  uint16_t set_v5v_conv = 11000;  // V5V conversion - 11000.

  // Open the I2C device:
  if (open_i2c_dev(&file_i2c, filename, i2c_addr) == 0)
  {
    printf("Opened I2C file: %s, Address: 0x%x\n", filename, i2c_addr);
  }
  else
  {
    printf("Failed to setup i2c\n");
    return -1;
  }

  // New line spacing..
  printf("\n\n");
  // Print current converstion constants:
  printf ("==========================================================\n");
  printf ("            CURRENT CONVERSION CONSTANTS:                 \n");
  printf ("==========================================================\n");
  printf ("\n");

  if (piups_get_vbattconv(file_i2c, &read_vbatt_conv) == 0)
  {
    printf("  VBatt: %i. \n", read_vbatt_conv);
  }
  else
  {
    printf("  VBatt: Failed to read\n"); 
  }
  usleep (1000); 

  if (piups_get_vrailconv(file_i2c, &read_vrail_conv) == 0)
  {
    printf("  VRail: %i. \n", read_vrail_conv);
  }
  else
  {
    printf("  VRail: Failed to read\n"); 
  }
  usleep (1000); 

  if (piups_get_vaux1conv(file_i2c, &read_vaux1_conv) == 0)
  {
    printf("  VAux1: %i. \n", read_vaux1_conv);
  }
  else
  {
    printf("  VAux1: Failed to read\n"); 
  }
  usleep (1000); 

  if (piups_get_vaux2conv(file_i2c, &read_vaux2_conv) == 0)
  {
    printf("  VAux2: %i. \n", read_vaux2_conv);
  }
  else
  {
    printf("  VAux2: Failed to read\n"); 
  }
  usleep (1000); 

  if (piups_get_v5vconv(file_i2c, &read_v5v_conv) == 0)
  {
    printf("  V5v:   %i. \n", read_v5v_conv);
  }
  else
  {
    printf("  V5v:   Failed to read\n"); 
  }
  usleep (1000); 
  
  // Set new conversion constants
  printf ("\n\n");
  printf ("==========================================================\n");
  printf ("            SETTING NEW CONVERSION CONSTANTS:             \n");
  printf ("==========================================================\n");
  printf ("\n");
  
  if (set_vbatt_conv == 0)
  {
    printf("  VBatt: SKIP\n");
  } 
  else if(piups_set_vbattconv(file_i2c, set_vbatt_conv) == 0)
  {
    printf("  VBatt: SET to %i\n", set_vbatt_conv);
  }
  else
  {
    printf("  VBatt: Failed to write\n");
  }
  usleep (1000); 

  if (set_vrail_conv == 0)
  {
    printf("  VRail: SKIP\n");
  } 
  else if(piups_set_vrailconv(file_i2c, set_vrail_conv) == 0)
  {
    printf("  VRail: SET to %i\n", set_vrail_conv);
  }
  else
  {
    printf("  VRail: Failed to write\n");
  }
  usleep (1000); 

  if (set_vaux1_conv == 0)
  {
    printf("  VAUX1: SKIP\n");
  } 
  else if(piups_set_vaux1conv(file_i2c, set_vaux1_conv) == 0)
  {
    printf("  VAUX1: SET to %i\n", set_vaux1_conv);
  }
  else
  {
    printf("  VAUX1: Failed to write\n");
  } 
  usleep (1000); 

  if (set_vaux2_conv == 0)
  {
    printf("  VAUX2: SKIP\n");
  } 
  else if(piups_set_vaux2conv(file_i2c, set_vaux2_conv) == 0)
  {
    printf("  VAUX2: SET to %i\n", set_vaux2_conv);
  }
  else
  {
    printf("  VAUX2: Failed to write\n");
  }
  usleep (1000); 
  
  if (set_v5v_conv == 0)
  {
    printf("  V5V:   SKIP\n");
  } 
  else if(piups_set_v5vconv(file_i2c, set_v5v_conv) == 0)
  {
    printf("  V5V:   SET to %i\n", set_v5v_conv);
  }
  else
  {
    printf("  V5V:   Failed to write\n");
  }
  usleep (1000); 


  // Print current converstion constants:
  printf ("\n\n");
  printf ("==========================================================\n");
  printf ("            READ NEW CONVERSION CONSTANTS:                \n");
  printf ("==========================================================\n");
  printf ("\n");
  // TODO: also include checks to make sure set values match..

  if (piups_get_vbattconv(file_i2c, &read_vbatt_conv) == 0)
  {
    printf("  VBatt: %i. \n", read_vbatt_conv);
  }
  else
  {
    printf("  VBatt: Failed to read\n"); 
  }
  usleep (1000); 

  if (piups_get_vrailconv(file_i2c, &read_vrail_conv) == 0)
  {
    printf("  VRail: %i. \n", read_vrail_conv);
  }
  else
  {
    printf("  VRail: Failed to read\n"); 
  }
  usleep (1000); 

  if (piups_get_vaux1conv(file_i2c, &read_vaux1_conv) == 0)
  {
    printf("  VAux1: %i. \n", read_vaux1_conv);
  }
  else
  {
    printf("  VAux1: Failed to read\n"); 
  }
  usleep (1000); 

  if (piups_get_vaux2conv(file_i2c, &read_vaux2_conv) == 0)
  {
    printf("  VAux2: %i. \n", read_vaux2_conv);
  }
  else
  {
    printf("  VAux2: Failed to read\n"); 
  }
  usleep (1000);  

  if (piups_get_v5vconv(file_i2c, &read_v5v_conv) == 0)
  {
    printf("  V5v:   %i. \n", read_v5v_conv);
  }
  else
  {
    printf("  V5v:   Failed to read\n"); 
  }
  usleep (1000); 

  return 0;
}
