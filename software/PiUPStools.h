#ifndef PIUPSTOOLS_H
#define PIUPSTOOLS_H
/** \file
 *  \brief  PiUPS Utility functions
 *  A set of utility functions for accessing the PiUPS
 *  registers over the I2C bus.
 */


#include "i2ctools.h"


/// Print the current status of the ups system:
/* Function to print the current status of the ups system to screen
 * @param file_i2c I2C file connected to the PiUPS device.
 */
void piups_printstats(int file_i2c);


/// Get the current voltage of the microprocessor VCC in mV
/* Function to get the current VCC voltage as read by
 * the ADC.
 * @param file_i2c I2C file connected to the PiUPS device
 * @param vcc pointer to return value of vcc
 * @return 0 on success
 */
int piups_getvcc(int file_i2c, int *vcc);




#endif

