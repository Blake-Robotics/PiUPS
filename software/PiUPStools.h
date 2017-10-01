#ifndef PIUPSTOOLS_H
#define PIUPSTOOLS_H
/** \file
 *  \brief  PiUPS Utility functions
 *  A set of utility functions for accessing the PiUPS
 *  registers over the I2C bus.
 */


#include "i2ctools.h"
#include "PiUPSDefines.h"

/// Print the current status of the ups system:
/** Function to print the current status of the ups system to screen
 * @param file_i2c I2C file connected to the PiUPS device.
 */
void piups_printstats(int file_i2c);


/// Get the current voltage of the microprocessor VCC in mV
/** Function to get the current VCC voltage as read by
 * the ADC.
 * @param file_i2c I2C file connected to the PiUPS device
 * @param vcc pointer to return value of vcc
 * @return 0 on success
 */
int piups_getvcc(int file_i2c, uint16_t *vcc);


/// Get the current status of the UPS:
/** Function to get the current status of the UPS system.
 *
 * @param file_i2c I2C file connected to the PiUPS device
 * @param bat_status Status of the connected battery
 * @param pwr_src Current power source for the PiUPS
 * @param pwr_rail CUrrent power source for the output rail
 * @return 0 on success
 */
int piups_getstatus(int file_i2c, PiUPSBattery* bat_status, PiUPSPower* pwr_src, PiUPSPower* rail_src);

#endif

