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
 * @param file I2C file connected to the PiUPS device.
 */
void piups_printstats(int file);







#endif

