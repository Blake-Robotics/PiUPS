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
  char *filename = (char*)"/dev/i2c-1";
  const uint8_t i2c_addr = 0x13;

  // Parameters to read
  uint16_t read_vbatt_conv, read_vrail_conv, read_vaux1_conv, read_vaux2_conv,
            read_v5v_conv, read_vauxo_conv, read_irail_conv, read_vbattlowdis,
            read_vbattlowen, read_vraillowsw, read_vrailhighsw, read_vrailcompen,
            read_raililim, read_chargexcess, read_maxcharge;
  
  // Parameters to set, set != 0 to set the parameter.
  // Conversion constants:
  uint16_t set_vbatt_conv = 11000; // VBatt Conversion - 11000
  uint16_t set_vrail_conv = 11000; // VRail Conversion - 11000
  uint16_t set_vaux1_conv = 11000; // VAUX1 Conversion - 11000
  uint16_t set_vaux2_conv = 11000; // VAUX2 Conversion - 11000
  uint16_t set_v5v_conv   = 11000; // V5V Conversion - 11000
  uint16_t set_vauxo_conv = 11000; // VAUXOut Conversion - 11000 
  uint16_t set_irail_conv = 3333;  // IRAIL conversion - 3333
  // Configuration constants:
  uint16_t set_vbattlowdis = 2900*3; // LiFePo4 : 2800mV
  uint16_t set_vbattlowen =  set_vbattlowdis ? set_vbattlowdis + 300 : 0;
  uint16_t set_vraillowsw =  0;
  uint16_t set_vrailhighsw =  0;
  uint16_t set_vrailcompen =  0;
  uint16_t set_raililim =  8000;
  uint16_t set_chargexcess =  1000;
  uint16_t set_maxcharge =  3500*3; // LiFePo4 : 3600mV
  

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
    printf("  V 5V: %i. \n", read_v5v_conv);
  }
  else
  {
    printf("  V 5V: Failed to read\n"); 
  }
  usleep (1000); 
  
  if (piups_get_vauxoconv(file_i2c, &read_vauxo_conv) == 0)
  {
    printf("  VAuxO: %i. \n", read_vauxo_conv);
  }
  else
  {
    printf("  VAuxO: Failed to read\n"); 
  }
  usleep (1000); 
  

  if (piups_get_irailconv(file_i2c, &read_irail_conv) == 0)
  {
    printf("  IRail: %i. \n", read_irail_conv);
  }
  else
  {
    printf("  IRail: Failed to read\n"); 
  }
  usleep (1000); 
  
  if (read_i2c_reg(file_i2c, PiUPSVBattLowDis, &read_vbattlowdis) == 0)
  {
    printf("  VBattLowDis: %i. \n", read_vbattlowdis);
  }
  else
  {
    printf("  VBattLowDis: Failed to read\n"); 
  }
  usleep (1000);
  
  if (read_i2c_reg(file_i2c, PiUPSVBattLowEn, &read_vbattlowen) == 0)
  {
    printf("  VBattLowEn: %i. \n", read_vbattlowen);
  }
  else
  {
    printf("  VBattLowEn: Failed to read\n"); 
  }
  usleep (1000);
  
  if (read_i2c_reg(file_i2c, PiUPSRailLowSw, &read_vraillowsw) == 0)
  {
    printf("  RailLowSw: %i. \n", read_vraillowsw);
  }
  else
  {
    printf("  RailLowSw: Failed to read\n"); 
  }
  usleep (1000); 
  
  if (read_i2c_reg(file_i2c, PiUPSRailHighSw, &read_vrailhighsw) == 0)
  {
    printf("  RailHighSw: %i. \n", read_vrailhighsw);
  }
  else
  {
    printf("  RailHighSw: Failed to read\n"); 
  }
  usleep (1000);
  
  if (read_i2c_reg(file_i2c, PiUPSRailCompEn, &read_vrailcompen) == 0)
  {
    printf("  RailCompEn: %i. \n", read_vrailcompen);
  }
  else
  {
    printf("  RailCompEn: Failed to read\n"); 
  }
  usleep (1000); 
  
  if (read_i2c_reg(file_i2c, PiUPSIRailLim, &read_raililim) == 0)
  {
    printf("  IRailLim: %i. \n", read_raililim);
  }
  else
  {
    printf("  IRailLim: Failed to read\n"); 
  }
  usleep (1000);
  
  if (read_i2c_reg(file_i2c, PiUPSChargeExcess, &read_chargexcess) == 0)
  {
    printf("  ChargeExcess: %i. \n", read_chargexcess);
  }
  else
  {
    printf("  ChargeExcess: Failed to read\n"); 
  }
  usleep (1000);
  
  if (read_i2c_reg(file_i2c, PiUPSChargeLimit, &read_maxcharge) == 0)
  {
    printf("  ChargeLimit: %i. \n", read_chargexcess);
  }
  else
  {
    printf("  ChargeLimit: Failed to read\n"); 
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
    printf("  V 5V: SKIP\n");
  } 
  else if(piups_set_v5vconv(file_i2c, set_v5v_conv) == 0)
  {
    printf("  V 5V: SET to %i\n", set_v5v_conv);
  }
  else
  {
    printf("  VAUX2: Failed to write\n");
  }
  usleep (1000); 

  if (set_vauxo_conv == 0)
  {
    printf("  VAUXO: SKIP\n");
  } 
  else if(piups_set_vauxoconv(file_i2c, set_vauxo_conv) == 0)
  {
    printf("  VAUXO: SET to %i\n", set_vauxo_conv);
  }
  else
  {
    printf("  VAUXO: Failed to write\n");
  }
  usleep (1000); 

  if (set_irail_conv == 0)
  {
    printf("  IRAIL: SKIP\n");
  } 
  else if(piups_set_irailconv(file_i2c, set_irail_conv) == 0)
  {
    printf("  IRAIL: SET to %i\n", set_irail_conv);
  }
  else
  {
    printf("  IRAIL: Failed to write\n");
  }
  usleep (1000);
  
  if (set_vbattlowdis == 0)
  {
    printf("  VBattLowDis: SKIP\n");
  } 
  else if(write_i2c_reg(file_i2c, PiUPSVBattLowDis, set_vbattlowdis) == 0)
  {
    printf("  VBattLowDis: SET to %i\n", set_vbattlowdis);
  }
  else
  {
    printf("  VBattLowDis: Failed to write\n");
  }
  usleep (1000);
  
  if (set_vbattlowen == 0)
  {
    printf("  VBattLowEn: SKIP\n");
  } 
  else if(write_i2c_reg(file_i2c, PiUPSVBattLowEn, set_vbattlowen) == 0)
  {
    printf("  VBattLowEn: SET to %i\n", set_vbattlowen);
  }
  else
  {
    printf("  VBattLowEn: Failed to write\n");
  }
  usleep (1000);
  
  if (set_vraillowsw == 0)
  {
    printf("  RailLowSw: SKIP\n");
  } 
  else if(write_i2c_reg(file_i2c, PiUPSRailLowSw, set_vraillowsw) == 0)
  {
    printf("  RailLowSw: SET to %i\n", set_vraillowsw);
  }
  else
  {
    printf("  RailLowSw: Failed to write\n");
  }
  usleep (1000); 
  
  if (set_vrailhighsw == 0)
  {
    printf("  RailHighSw: SKIP\n");
  } 
  else if(write_i2c_reg(file_i2c, PiUPSRailHighSw, set_vrailhighsw) == 0)
  {
    printf("  RailHighSw: SET to %i\n", set_vrailhighsw);
  }
  else
  {
    printf("  RailHighSw: Failed to write\n");
  }
  usleep (1000);
  
  if (set_vrailcompen == 0)
  {
    printf("  RailCompEn: SKIP\n");
  } 
  else if(write_i2c_reg(file_i2c, PiUPSRailCompEn, set_vrailcompen) == 0)
  {
    printf("  RailCompEn: SET to %i\n", set_vrailcompen);
  }
  else
  {
    printf("  RailCompEn: Failed to write\n");
  }
  usleep (1000); 
  
  if (set_raililim == 0)
  {
    printf("  IRailLim: SKIP\n");
  } 
  else if(write_i2c_reg(file_i2c, PiUPSIRailLim, set_raililim) == 0)
  {
    printf("  IRailLim: SET to %i\n", set_raililim);
  }
  else
  {
    printf("  IRailLim: Failed to write\n");
  }
  usleep (1000);
  
  if (set_chargexcess == 0)
  {
    printf("  ChargeExcess: SKIP\n");
  } 
  else if(write_i2c_reg(file_i2c, PiUPSChargeExcess, set_chargexcess) == 0)
  {
    printf("  ChargeExcess: SET to %i\n", set_chargexcess);
  }
  else
  {
    printf("  ChargeExcess: Failed to write\n");
  }
  usleep (1000);
  
  if (set_maxcharge == 0)
  {
    printf("  ChargeLimit: SKIP\n");
  } 
  else if(write_i2c_reg(file_i2c, PiUPSChargeLimit, set_maxcharge) == 0)
  {
    printf("  ChargeLimit: SET to %i\n", set_maxcharge);
  }
  else
  {
    printf("  ChargeLimit: Failed to write\n");
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
    printf("  V 5V: %i. \n", read_v5v_conv);
  }
  else
  {
    printf("  V 5V: Failed to read\n"); 
  }
  usleep (1000); 
  if (piups_get_vauxoconv(file_i2c, &read_vauxo_conv) == 0)
  {
    printf("  VAuxO: %i. \n", read_vauxo_conv);
  }
  else
  {
    printf("  VAuxO: Failed to read\n"); 
  }
  usleep (1000); 
  

  if (piups_get_irailconv(file_i2c, &read_irail_conv) == 0)
  {
    printf("  IRail: %i. \n", read_irail_conv);
  }
  else
  {
    printf("  IRail: Failed to read\n"); 
  }
  usleep (1000); 
  
  if (read_i2c_reg(file_i2c, PiUPSVBattLowDis, &read_vbattlowdis) == 0)
  {
    printf("  VBattLowDis: %i. \n", read_vbattlowdis);
  }
  else
  {
    printf("  VBattLowDis: Failed to read\n"); 
  }
  usleep (1000);
  
  if (read_i2c_reg(file_i2c, PiUPSVBattLowEn, &read_vbattlowen) == 0)
  {
    printf("  VBattLowEn: %i. \n", read_vbattlowen);
  }
  else
  {
    printf("  VBattLowEn: Failed to read\n"); 
  }
  usleep (1000);
  
  if (read_i2c_reg(file_i2c, PiUPSRailLowSw, &read_vraillowsw) == 0)
  {
    printf("  RailLowSw: %i. \n", read_vraillowsw);
  }
  else
  {
    printf("  RailLowSw: Failed to read\n"); 
  }
  usleep (1000); 
  
  if (read_i2c_reg(file_i2c, PiUPSRailHighSw, &read_vrailhighsw) == 0)
  {
    printf("  RailHighSw: %i. \n", read_vrailhighsw);
  }
  else
  {
    printf("  RailHighSw: Failed to read\n"); 
  }
  usleep (1000);
  
  if (read_i2c_reg(file_i2c, PiUPSRailCompEn, &read_vrailcompen) == 0)
  {
    printf("  RailCompEn: %i. \n", read_vrailcompen);
  }
  else
  {
    printf("  RailCompEn: Failed to read\n"); 
  }
  usleep (1000); 
  
  if (read_i2c_reg(file_i2c, PiUPSIRailLim, &read_raililim) == 0)
  {
    printf("  IRailLim: %i. \n", read_raililim);
  }
  else
  {
    printf("  IRailLim: Failed to read\n"); 
  }
  usleep (1000);
  
  if (read_i2c_reg(file_i2c, PiUPSChargeExcess, &read_chargexcess) == 0)
  {
    printf("  ChargeExcess: %i. \n", read_chargexcess);
  }
  else
  {
    printf("  ChargeExcess: Failed to read\n"); 
  }
  usleep (1000);
  
  if (read_i2c_reg(file_i2c, PiUPSChargeLimit, &read_maxcharge) == 0)
  {
    printf("  ChargeLimit: %i. \n", read_chargexcess);
  }
  else
  {
    printf("  ChargeLimit: Failed to read\n"); 
  }
  usleep (1000);
  
  return 0;
}
