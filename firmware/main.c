/* tinyBuck firmware
 *
 * Firmware to drive the tinyBuck led patterns. Provides an
 * i2c interface and basic PWM output. Parameters are stored
 * in the EEPROM.
 *
 * Chip Pinout:
 *   
 *  PA0 - PWM0
 *  PA1 - PWM1
 *  PA2 - PWM2
 *
 *
 */


/*****************************************************************************
 * Includes
 *****************************************************************************/ 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <string.h>
#include <stdbool.h>

// I2C Slave:
#include "usitwislave.h"

// Project includes
#include "PiUPSDefines.h"
#include "eeprom.h"

/*****************************************************************************
 * Global Variables & Functions
 *****************************************************************************/

// I2C polling - check for new data recieved
void i2c_recv_callback(uint8_t input_buffer_length, const uint8_t *input_buffer,
				uint8_t *output_buffer_length, uint8_t *output_buffer);				
void i2c_idle_callback(void);

// Enable/disable the low voltage sensing on the main power rail.
void ANACOMP_Enable(void);
void ANACOMP_Disable(void);

// Timer 1 - Used to keep track of global time.
void EnableTimer1(void);
volatile uint16_t extended_time;
uint32_t CurrentTime(void);

// ADC Handling:
void ADCStateHandler(void);
volatile PiUPSADCState CurrentADCState;
uint32_t ADCWaitTime;

// ADC converted voltages:
uint16_t voltage_vcc;
uint16_t voltage_bat;
uint16_t voltage_rail;
uint16_t voltage_aux1;
uint16_t voltage_aux2;

// Rail divisions..
uint16_t div_bat = 4880;
uint16_t div_rail = 4880;

volatile PiUPSBattery BatteryStatus = 0x0;
volatile PiUPSPower PowerStatus = 0x0;
bool PowerSave = false;


/*****************************************************************************
 * Main Program
 *****************************************************************************/
int main (void)
{
    
    // Setup port B for output:
    DDRB |= (1 << PB0); 
    PORTB |= (1 << PB0);

    // Put ADC into starting state:
    CurrentADCState = ADCInit;
    
    // Enable Global Timekeeping:
    EnableTimer1();
    
    //ANACOMP_Enable();
    
    sei();
    
    // use I2C:
    usi_twi_slave(0x13, 0, &i2c_recv_callback, &i2c_idle_callback);
};

/*****************************************************************************
 * ADC Conversion handling:
 *****************************************************************************/
// Function to handle the current state of the ADC State
// Machine.. 
void ADCStateHandler(void)
{

  switch (CurrentADCState)
  {
    // Default option is to drop into an error state
    default:
      CurrentADCState = ADCERR;
      break;
    
    // Idle state is to do nothing.
    case ADCIdle:
      break;
      
    // Error state does nothing.
    case ADCERR:
      break;

    // Start the ADC initilisaton sequence:
    case ADCInit:
      ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // enable adc with clk/64 (8MHz -> 125kHz)
      ADMUX = (1 << MUX5) | (1 << MUX0); // Set mux to 1.1 bandgap reference.
      ADCWaitTime = CurrentTime() + 32E3; // Set next state to wait 32,000 clock cycles.
      CurrentADCState = ADCInitWait;
      break;
    
    // Wait for voltages to settle before starting the first conversion
    // Note that reading the internal reference voltage requires some
    // time to stabilise, which is the reason for the wait state.
    case ADCInitWait:
      if ( CurrentTime() > ADCWaitTime)
      {
        ADCSRA |= (1 << ADSC);
        CurrentADCState = ADCVccWait;
      }
      break;
      
    // Wait for conversion to finish, then store VCC value:
    case ADCVccWait:
      if ( !(ADCSRA & (1 << ADSC) ) )
      {
        voltage_vcc = (((uint32_t)1100*1024)/ ADC);
        ADMUX = (1 << MUX0); // Set the mux to ADC1
        ADCSRA |= (1 << ADSC);
        CurrentADCState = ADCVbatWait;
      }
      break;
      
    // Wait for conversion, then store Vbat
    case ADCVbatWait:
      if ( !(ADCSRA & (1 << ADSC) ) )
      {
        // Convert the ADC value to a voltage in mV, using
        // the stored value for the resistor divider.
        voltage_bat = ((uint32_t)voltage_vcc*ADC/1024)*
                       read_eeprom_word(EEPROM_VBATT_CONV)/1000;
        
        ADMUX = (1 << MUX1); // Set the mux to ADC2
        ADCSRA |= (1 << ADSC);
        CurrentADCState = ADCVrailWait;
      }
      break;
    
    // Wait for conversion, then store Vrail 
    case ADCVrailWait:
      if ( !(ADCSRA & (1 << ADSC) ) )
      {
        voltage_rail = ((uint32_t)voltage_vcc*ADC/1024)*
                        read_eeprom_word(EEPROM_VRAIL_CONV)/1000;
        
        ADMUX = (1 << MUX1) | (1 << MUX0); // Set the mux to ADC3
        ADCSRA |= (1 << ADSC);
        CurrentADCState = ADCVauxlWait;
      }
      break;
      
    // Wait for conversion, then store Vaux1
    case ADCVauxlWait:
      if ( !(ADCSRA & (1 << ADSC) ) )
      {
        voltage_aux1 = ((uint32_t)voltage_vcc*ADC/1024)*
                        read_eeprom_word(EEPROM_VAUX1_CONV)/1000;
        
        ADMUX = (1 << MUX2); // Set the mux to ADC4
        ADCSRA |= (1 << ADSC);
        CurrentADCState = ADCVaux2Wait;
      }
      break;
      
    // Wait for conversion, then store Vaux2
    case ADCVaux2Wait:
      if ( !(ADCSRA & (1 << ADSC) ) )
      {
        voltage_aux2 = ((uint32_t)voltage_vcc*ADC/1024)*
                        read_eeprom_word(EEPROM_VAUX2_CONV)/1000;
        CurrentADCState = ADCConvComplete;
      }
      break;
      
    case ADCConvComplete:
      CurrentADCState = ADCInit;
      break;
      
    case ADCDisable:
    
      break;
  }
}

/*****************************************************************************
 * Timer 1: Global Timekeeping 
 *****************************************************************************/
void EnableTimer1(void)
{
    // Setup 16-bit timer to keep track of internal time:
    TCCR1A = 0x0;       // Normal operation
    TCCR1B = (1<<CS10); // Set prescaler to 1.
    TIMSK1 |= (1<<TOIE1); // Interrupt on overflow
    TCNT1 = 0;
}

// Global timer handler:
ISR(TIM1_OVF_vect) {
    extended_time++;
}

uint32_t CurrentTime(void)
{
  return ((uint32_t) extended_time << 16) + TCNT1;
}
 

/*****************************************************************************
 * I2C Callbacks
 *****************************************************************************/

// Handle requests for data from the I2C bus:
void i2c_recv_callback(uint8_t input_buffer_length, const uint8_t *input_buffer,
				uint8_t *output_buffer_length, uint8_t *output_buffer)
{
  uint16_t temp;


  // If a single byte has been recieved, then it is specifying a register
  // to read on the next transaction, fill the output buffer accordingly...
  if ( input_buffer_length == 1 )
  {     
    switch (input_buffer[0])
    {
    
      // If incorrect register is specified, read nothing:
      default:
        *output_buffer_length = 0;
        return;
        
      // Current Status:
      
        
      // Voltage Reading:
      case PIUPS_VCC:
        output_buffer[0] = (voltage_vcc >> 8);
        output_buffer[1] = (voltage_vcc);
        break;
        
      case PIUPS_VBATT:
        output_buffer[0] = (voltage_bat >> 8);
        output_buffer[1] = (voltage_bat);
        break;
        
      case PIUPS_VRAIL:
        output_buffer[0] = (voltage_rail >> 8);
        output_buffer[1] = (voltage_rail);
        break;
        
      case PIUPS_VAUX1:
        output_buffer[0] = (voltage_aux1 >> 8);
        output_buffer[1] = (voltage_aux1);
        break;
        
      case PIUPS_VAUX2:
        output_buffer[0] = (voltage_aux2 >> 8);
        output_buffer[1] = (voltage_aux2);
        break;
      
      // Conversion Factors:
      case PIUPS_VBATT_CONV:
        temp = read_eeprom_word (EEPROM_VBATT_CONV);
        output_buffer[0] = (temp >> 8);
        output_buffer[1] = (temp);
        break;
        
      case PIUPS_VRAIL_CONV:
        temp = read_eeprom_word (EEPROM_VRAIL_CONV);
        output_buffer[0] = (temp >> 8);
        output_buffer[1] = (temp);
        break;
        
      case PIUPS_VAUX1_CONV:
        temp = read_eeprom_word (PIUPS_VAUX1_CONV);
        output_buffer[0] = (temp >> 8);
        output_buffer[1] = (temp);
        break;
        
      case PIUPS_VAUX2_CONV:
        temp = read_eeprom_word (PIUPS_VAUX2_CONV);
        output_buffer[0] = (temp >> 8);
        output_buffer[1] = (temp);
        break;
    }
    
    *output_buffer_length = 2;

  }
  
  // If three bytes have been written, then an address followed
  // by two data bytes have been written:
  else if ( input_buffer_length == 3 )
  {
    switch (input_buffer[0])
    {
      default:
        break;
        
      case PIUPS_VBATT_CONV:
        update_eeprom_byte (EEPROM_VBATT_CONV+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_VBATT_CONV, input_buffer[2]);
        break;
      
      case PIUPS_VRAIL_CONV:
        update_eeprom_byte (EEPROM_VRAIL_CONV+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_VRAIL_CONV, input_buffer[2]);
        break;
        
      case PIUPS_VAUX1_CONV:
        update_eeprom_byte (EEPROM_VAUX1_CONV+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_VAUX1_CONV, input_buffer[2]);
        break;
        
      case PIUPS_VAUX2_CONV:
        update_eeprom_byte (EEPROM_VAUX2_CONV+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_VAUX2_CONV, input_buffer[2]);
        break;
        
      case PIUPS_VBATT_LOWDIS:
        update_eeprom_byte (EEPROM_VBATT_LOWDIS+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_VBATT_LOWDIS, input_buffer[2]);
        break;
      
      case PIUPS_VBATT_LOWEN:
        update_eeprom_byte (EEPROM_VBATT_LOWDIS+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_VBATT_LOWDIS, input_buffer[2]);
        break;
     
    }
    *output_buffer_length = 0;
  
  
  }
  else
  {
    // Nothing to
    *output_buffer_length = 0;
  }
  


}

// Idle callback, should do housekeeping and return
// quickly to make sure I2C is accessible.
void i2c_idle_callback(void)
{
    if ((ACSR & (1<<ACO)) == 0)
    {
        PORTB &= ~(1 << PB0); // PD0 goes low
    }

    ADCStateHandler ();
}

/*****************************************************************************
 * Comparator operation
 *****************************************************************************/

void ANACOMP_Enable(void)
{ 
    ACSR = (1 << ACBG) | (1 << ACIE) | (1 << ACIS1) | (1 << ACIS0);
}
void ANACOMP_Disable(void)
{
    ACSR = (1 << ACD);
}

ISR(ANA_COMP_vect)
{
    // If ACO is 1, then - > + in the comparator, meaning the sensed voltage
    // has dropped and the battery circuit should be enabled, if available.
    if ( (ACSR & (1<<ACO)) && ((BatteryStatus & PiUPSBatteryLow)==0x0) )
    {
        PORTB |= (1 << PB0); // Enable battery.
        PowerStatus = PiUPSPowerBatt;
    }
}


