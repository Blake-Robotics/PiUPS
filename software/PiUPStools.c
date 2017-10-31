#include "PiUPStools.h"
#include <stdio.h> 
#include <stdint.h>
#include <unistd.h>
#include "PiUPSDefines.h"

//////////////////////////////////////////////////////////////////////////
// Functions to handle setting/getting of voltage conversion parameters.
int piups_set_vbattconv(int file_i2c, uint16_t vbattconv)
{
  return write_i2c_reg(file_i2c, PIUPS_VBATT_CONV, vbattconv);
}
int piups_get_vbattconv(int file_i2c, uint16_t *vbattconv)
{
  return read_i2c_reg(file_i2c, PIUPS_VBATT_CONV, vbattconv);
}

int piups_set_vrailconv(int file_i2c, uint16_t vrailconv)
{
  return write_i2c_reg(file_i2c, PIUPS_VRAIL_CONV, vrailconv);
}
int piups_get_vrailconv(int file_i2c, uint16_t *vrailconv)
{
  return read_i2c_reg(file_i2c, PIUPS_VRAIL_CONV, vrailconv);
}

int piups_set_vaux1conv(int file_i2c, uint16_t vaux1conv)
{
  return write_i2c_reg(file_i2c, PIUPS_VAUX1_CONV, vaux1conv);
}
int piups_get_vaux1conv(int file_i2c, uint16_t *vaux1conv)
{
  return read_i2c_reg(file_i2c, PIUPS_VAUX1_CONV, vaux1conv);
}

int piups_set_vaux2conv(int file_i2c, uint16_t vaux2conv)
{
  return write_i2c_reg(file_i2c, PIUPS_VAUX2_CONV, vaux2conv);
}
int piups_get_vaux2conv(int file_i2c, uint16_t *vaux2conv)
{
  return read_i2c_reg(file_i2c, PIUPS_VAUX2_CONV, vaux2conv);
}

int piups_set_v5vconv(int file_i2c, uint16_t v5vconv)
{
  return write_i2c_reg(file_i2c, PIUPS_V5V_CONV, v5vconv);
}
int piups_get_v5vconv(int file_i2c, uint16_t *v5vconv)
{
  return read_i2c_reg(file_i2c, PIUPS_V5V_CONV, v5vconv);
}


/////////////////////////////////////////////////////////////////////////
void piups_printstats(int file_i2c)
{
  uint16_t vcc, vbat, vrail, vaux1, vaux2, v5v, status; 
  
  if (read_i2c_reg(file_i2c, PIUPS_STATUS, &status) == 0)
  {
    printf("Read Status as as 0x%x. \n", status);
  }
  else
  {
    printf("Failed to read Status \n");
  }

  usleep (5000);

  if (read_i2c_reg(file_i2c, PIUPS_VCC, &vcc) == 0)
  {
    printf("Read VCC as %d mV. \n", vcc);
  }
  else
  {
    printf("Failed to read VCC \n");
  }

  usleep(5000); 
 
  if (read_i2c_reg(file_i2c, PIUPS_V5V, &v5v) == 0)
  {
    printf("Read 5V output as %d mV. \n", v5v);
  }
  else
  {
    printf("Failed to read 5V output \n");
  }


  usleep (5000);
	
  if (read_i2c_reg(file_i2c, PIUPS_VBATT, &vbat) == 0)
  {
    printf("Read VBatt as %d mV. \n", vbat);
  }
  else
  {
    printf("Failed to read VBatt \n");
  }

  usleep (5000);	

  if (read_i2c_reg(file_i2c, PIUPS_VRAIL, &vrail) == 0)
  {
    printf("Read VRail as %d mV. \n", vrail);
  }
  else
  { 
    printf("Failed to read VRail \n");
  }

  usleep (5000);	
  
  if (read_i2c_reg(file_i2c, PIUPS_VAUX1, &vaux1) == 0)
  {
    printf("Read VAUX1 as %d mV. \n", vaux1);
  }
  else
  { 
    printf("Failed to read VAUX1 \n");
  }
  
  usleep (5000);	
  
  if (read_i2c_reg(file_i2c, PIUPS_VAUX2, &vaux2) == 0)
  {
    printf("Read VAUX2 as %d mV. \n", vaux2);
  }
  else
  { 
    printf("Failed to read VAUX2 \n");
  }

  PiUPSBattery bat_status;
  PiUPSPower pwr_src, rail_src;
  if (piups_getstatus(file_i2c, &bat_status, &pwr_src, &rail_src) == 0)
  {
    printf("Read Status as: \n");
    printf(" + Battery Status:\n");
    if (bat_status & PiUPSBatteryLow) printf("    + Low Battery\n"); 
    if (bat_status & PiUPSBatteryGood) printf("    + Good Battery\n");
    if (bat_status & PiUPSBatteryChg) printf("    + Charging Battery\n");
    
    printf(" + Power Source:\n");
    if (pwr_src & PiUPSPowerBatt) printf("    + Battery\n");
    if (pwr_src & PiUPSPowerAUX1) printf("    + AUX1\n");
    if (pwr_src & PiUPSPowerAUX2) printf("    + AUX2\n");
    
    printf(" + Rail Source:\n");
    if (rail_src & PiUPSPowerBatt) printf("    + Battery\n");
    if (rail_src & PiUPSPowerAUX1) printf("    + AUX1\n");
    if (rail_src & PiUPSPowerAUX2) printf("    + AUX2\n");
  }
  else
  {
    printf("Failed to read Status \n");
  }


  return 0;
}


int piups_getvcc(int file_i2c, uint16_t *vcc)
{
  
  return read_i2c_reg(file_i2c, PIUPS_VCC, vcc);

}

 
int piups_getstatus(int file_i2c, PiUPSBattery* bat_status, PiUPSPower* pwr_src, PiUPSPower* rail_src)
{
  int rval;
  uint16_t read_val;
  
  rval = read_i2c_reg(file_i2c, PIUPS_STATUS, &read_val);
  
  // convert values if successful:
  if (rval == 0)
  {
    *bat_status = read_val & 0xF;
    *pwr_src = (read_val >> 4) & 0xF;
    *rail_src = (read_val >> 8) & 0xF;
  }

  return rval;

}
