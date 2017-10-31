#ifndef PIUPSTOOLS_H
#define PIUPSTOOLS_H
/** \file
 *  \brief  PiUPS Utility functions
 *  A set of utility functions for accessing the PiUPS
 *  registers over the I2C bus.
 */


#include "i2ctools.h"
#include "PiUPSDefines.h"

/// Function to set the battery voltage conversion constant
/** Function to set the battery converstion constant, used to 
 * convert from the ADC read voltage to the real battery voltage.
 * @param file_i2c I2C file connected to the PiUPS device.
 * @param vbattconv the battery conversion constant
 * @return 0 on success
 */
int piups_set_vbattconv(int file_i2c, uint16_t vbattconv);

/// Function to get the battery voltage conversion constant
/** Function to get the battery converstion constant, used to 
 * convert from the ADC read voltage to the real battery voltage.
 * @param file_i2c I2C file connected to the PiUPS device.
 * @param vbattconv the retrived battery conversion constant
 * @return 0 on success
 */
int piups_get_vbattconv(int file_i2c, uint16_t *vbattconv);

/// Function to set the rail voltage conversion constant
/** Function to set the rail converstion constant, used to 
 * convert from the ADC read voltage to the real rail voltage.
 * @param file_i2c I2C file connected to the PiUPS device.
 * @param vbattconv the battery conversion constant
 * @return 0 on success
 */
int piups_set_vrailconv(int file_i2c, uint16_t vrailconst);

/// Function to get the rail voltage conversion constant
/** Function to get the rail converstion constant, used to 
 * convert from the ADC read voltage to the real rail voltage.
 * @param file_i2c I2C file connected to the PiUPS device.
 * @param vbattconv the retrived battery conversion constant
 * @return 0 on success
 */
int piups_get_vrailconv(int file_i2c, uint16_t *vrailconv);

/// Function to set the aux1 voltage conversion constant
/** Function to set the aux1 converstion constant, used to 
 * convert from the ADC read voltage to the real aux1 voltage.
 * @param file_i2c I2C file connected to the PiUPS device.
 * @param vbattconv the battery conversion constant
 * @return 0 on success
 */
int piups_set_vaux1conv(int file_i2c, uint16_t vaux1const);

/// Function to get the aux1 voltage conversion constant
/** Function to get the aux1 converstion constant, used to 
 * convert from the ADC read voltage to the real aux1 voltage.
 * @param file_i2c I2C file connected to the PiUPS device.
 * @param vbattconv the retrived battery conversion constant
 * @return 0 on success
 */
int piups_get_vaux1conv(int file_i2c, uint16_t *vaux1conv);

/// Function to set the aux2 voltage conversion constant
/** Function to set the aux2 converstion constant, used to 
 * convert from the ADC read voltage to the real aux2 voltage.
 * @param file_i2c I2C file connected to the PiUPS device.
 * @param vbattconv the battery conversion constant
 * @return 0 on success
 */
int piups_set_vaux2conv(int file_i2c, uint16_t vaux2const);

/// Function to get the aux2 voltage conversion constant
/** Function to get the aux2 converstion constant, used to 
 * convert from the ADC read voltage to the real aux2 voltage.
 * @param file_i2c I2C file connected to the PiUPS device.
 * @param vbattconv the retrived battery conversion constant
 * @return 0 on success
 */
int piups_get_vaux2conv(int file_i2c, uint16_t *vaux2conv);

/// Function to set the 5v output rail voltage conversion constant
/** Function to set the 5v output rail converstion constant, used to 
 * convert from the ADC read voltage to the real output rail voltage.
 * @param file_i2c I2C file connected to the PiUPS device.
 * @param vbattconv the battery conversion constant
 * @return 0 on success
 */
int piups_set_v5vconv(int file_i2c, uint16_t v5vconst);

/// Function to get the 5v output rail voltage conversion constant
/** Function to get the 5v output rail converstion constant, used to 
 * convert from the ADC read voltage to the real 5v rail voltage.
 * @param file_i2c I2C file connected to the PiUPS device.
 * @param vbattconv the retrived battery conversion constant
 * @return 0 on success
 */
int piups_get_v5vconv(int file_i2c, uint16_t *v5v2conv);







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

