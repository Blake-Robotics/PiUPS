/* PiUPS firmware
 *
 * Firmware to drive the PiUPS, a microcontoller which can
 * manage the power supply to a raspberry pi, and switch
 * between battery and aux, while providing voltage monitoring
 * capability.
 *
 * It is the aim that in the event the battery drops below 
 * the low voltage cutout, the battery will be disabled
 * and the microcontroller put into a very low power
 * sleep mode, until the voltage is acceptable again.
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
#include "PiUPSHWDefines.h"
#include "eeprom.h"

/*****************************************************************************
 * Global Variables & Functions
 *****************************************************************************/

// I2C polling - check for new data recieved
void i2c_recv_callback(uint8_t input_buffer_length, const uint8_t *input_buffer,
				uint8_t *output_buffer_length, uint8_t *output_buffer);				
void i2c_idle_callback(void);

// Timer 1 - Used to keep track of global time.
void EnableTimer1(void);
volatile uint16_t extended_time;
uint32_t CurrentTime(void);

// ADC Handling:
void ADCStateHandler(void);
volatile PiUPSADCState CurrentADCState;
uint32_t ADCWaitTime;

// Enable/disable the low voltage sensing on the main power rail.
void ANACOMP_Enable(void);
void ANACOMP_Disable(void);

// ADC converted voltages:
const uint16_t voltage_vcc_const = 3300;
uint16_t voltage_vcc;
uint16_t voltage_bat;
uint16_t voltage_rail;
uint16_t voltage_aux1;
uint16_t voltage_aux2;
uint16_t voltage_5v;
uint16_t voltage_aout;
uint16_t current_rail; // Current drawn by rail in mA.

// Rail divisions..
//uint16_t div_bat = 4880;
//uint16_t div_rail = 4880;

// Power status / control:
volatile PiUPSBattery BatteryStatus = PiUPSBatteryLow;
volatile PiUPSPower PowerSource = 0x0;
volatile PiUPSPower RailSource = 0x0;
void UpdateStatus(void);
void UpdateBatteryStatus(void);
void UpdateBatteryState(void);
bool PowerSave = false;


/*****************************************************************************
 * Main Program
 *****************************************************************************/
int main (void)
{
    
    // Setup port B for output:
    //DDRB |= (1 << PB0); 
    //PORTB |= (1 << PB0);
    
    // Setup IO ports with default values:
    SetupIO();
    EnableBattMon();
    EnableCurrMon();
    EnableCharge();
    EnableAuxOut();

    // Put ADC into starting state:
    CurrentADCState = ADCInit;
    
    // Enable Global Timekeeping:
    EnableTimer1();
    
    // Startup green led:
    LEDGreenOn();
    
    //ANACOMP_Enable();
    
    sei();
    
    // use I2C:
    // Note: this function uses callbacks to run the rest of the 
    // program.
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
      ADMUX = ADCMUX_REF; // Set mux to 1.1 bandgap reference.
      ADCWaitTime = CurrentTime() + 10E3; // Set next state to wait 10,000 clock cycles (1.25ms).
      CurrentADCState = ADCInitWait;
      break;
    
    // Wait for voltages to settle before starting the first conversion
    // Note that reading the internal reference voltage requires some
    // time to stabilise, which is the reason for the wait state.
    case ADCInitWait:
      if ( CurrentTime() > ADCWaitTime)
      {
        ADCSRA |= (1 << ADSC); // Start conversion
        CurrentADCState = ADCVccWait;
      }
      break;
      
    // Wait for conversion to finish, then store VCC value:
    case ADCVccWait:
      if ( !(ADCSRA & (1 << ADSC) ) )
      {
        voltage_vcc = (((uint32_t)1100*1024)/ ADC);
        ADMUX = ADCMUX_SBATT; // Set the mux to ADC1
        ADCWaitTime = CurrentTime() + 1E3; // Set next state to wait 1,000 clock cycles.
        CurrentADCState = ADCVbatInit;
      }
      break;
      
    // Wait for conversion, then store Vbat
    case ADCVbatInit:
      if ( CurrentTime() > ADCWaitTime)
      {
        ADCSRA |= (1 << ADSC); // Start conversion
        CurrentADCState = ADCVbatWait;
      }
      break;
      
    // Wait for conversion, then store Vbat
    case ADCVbatWait:
      if ( !(ADCSRA & (1 << ADSC) ) )
      {
        // Convert the ADC value to a voltage in mV, using
        // the stored value for the resistor divider.
        voltage_bat = ((uint32_t)voltage_vcc_const*ADC/1024)*
                       read_eeprom_word(EEPROM_VBATT_CONV)/1000;
        
        ADMUX = ADCMUX_SPBUS; // Set the mux to ADC2
        ADCSRA |= (1 << ADSC);
        CurrentADCState = ADCVrailWait;
      }
      break;
    
    // Wait for conversion, then store Vrail 
    case ADCVrailWait:
      if ( !(ADCSRA & (1 << ADSC) ) )
      {
        voltage_rail = ((uint32_t)voltage_vcc_const*ADC/1024)*
                        read_eeprom_word(EEPROM_VRAIL_CONV)/1000;
        
        ADMUX = ADCMUX_SAUX1; // Set the mux to ADC3
        ADCSRA |= (1 << ADSC);
        CurrentADCState = ADCVauxlWait;
      }
      break;
      
    // Wait for conversion, then store Vaux1
    case ADCVauxlWait:
      if ( !(ADCSRA & (1 << ADSC) ) )
      {
        voltage_aux1 = ((uint32_t)voltage_vcc_const*ADC/1024)*
                        read_eeprom_word(EEPROM_VAUX1_CONV)/1000;
        
        ADMUX = ADCMUX_SAUX2; // Set the mux to ADC5
        ADCSRA |= (1 << ADSC);
        CurrentADCState = ADCVaux2Wait;
      }
      break;
      
    // Wait for conversion, then store Vaux2
    case ADCVaux2Wait:
      if ( !(ADCSRA & (1 << ADSC) ) )
      {
        voltage_aux2 = ((uint32_t)voltage_vcc_const*ADC/1024)*
                        read_eeprom_word(EEPROM_VAUX2_CONV)/1000;
        ADMUX = ADCMUX_S5V; // Set the mux to ADC7
        ADCSRA |= (1 << ADSC);
        CurrentADCState = ADCV5VWait;
      }
      break;
      
    // Wait for conversion, then store V5V
    case ADCV5VWait:
      if ( !(ADCSRA & (1 << ADSC) ) )
      {
        voltage_5v = ((uint32_t)voltage_vcc_const*ADC/1024)*
                       read_eeprom_word(EEPROM_V5V_CONV)/1000;
        ADMUX = ADCMUX_SAUXO; // Set the mux to ADC7
        ADCSRA |= (1 << ADSC);
        CurrentADCState = ADCVauxOWait;
      }
      break;
      
    case ADCVauxOWait:
      if ( !(ADCSRA & (1 << ADSC) ) )
      {
        voltage_aout = ((uint32_t)voltage_vcc_const*ADC/1024)*
                        read_eeprom_word(EEPROM_VAUXO_CONV)/1000;
        ADMUX = ADCMUX_SCURR; // Set the mux to ADC7
        ADCSRA |= (1 << ADSC);
        CurrentADCState = ADCVauxCurrWait;
      }
      break;
      
    case ADCVauxCurrWait:
      if ( !(ADCSRA & (1 << ADSC) ) )
      {
        // TODO: Remove hack values:
        //current_rail = ADC;//((uint32_t)voltage_vcc*ADC/1024)*333;
        current_rail = ((uint32_t)voltage_vcc_const*ADC*3333/1024/1000);
        //current_rail = ((uint32_t)voltage_vcc_const*ADC*read_eeprom_word(EEPROM_IRAIL_CONV)/1024/1000);
        CurrentADCState = ADCConvComplete;
      }
      break;
      
    case ADCConvComplete:
        UpdateStatus();
        CurrentADCState = ADCInit;
      break;
      
    case ADCDisable:
    
      break;
  }
}


/*****************************************************************************
 * Power state and system management:
 *****************************************************************************/
void UpdateBatteryStatus(void)
{
  // Battery Status:
  // Check the battery level, with hysterisis:
  // TODO: Seperate out 'LOW' to mean critical
  //        Good -> Power for all.
  //        Low  -> Critical low power, switching into low power mode
  //        Neither good/low -> Not enough to activate PI, but enough
  //        to keep the cpu running.
  if (BatteryStatus & PiUPSBatteryGood)
  {
    if (voltage_bat < /*read_eeprom_word (EEPROM_VBATT_LOWDIS)*/ 6000)
    {
      BatteryStatus &= ~((uint8_t)PiUPSBatteryGood);
      BatteryStatus |= ((uint8_t)PiUPSBatteryLow);
      LEDRedOn();
    }
  }
  else
  {
    if (voltage_bat > /*read_eeprom_word (EEPROM_VBATT_LOWEN)*/ 6500)
    {
      BatteryStatus |= ((uint8_t)PiUPSBatteryGood);
      BatteryStatus &= ~((uint8_t)PiUPSBatteryLow);
      LEDRedOff();
    }
  }
}

void UpdateBatteryState(void)
{
  // Charging Condition
  if (  ( RailSource & (PiUPSPowerAUX1 | PiUPSPowerAUX2) ) // External Power
     && (voltage_rail > voltage_bat + 1000)) // At least 1V of excess drop...
  {
    EnableCharge();
    BatteryStatus |= PiUPSBatteryChg;
    
    DisableBatt();
    BatteryStatus &= ~((uint8_t)PiUSPBatteryEn);
  }
  
  // Power rail condition
  else if (BatteryStatus & PiUPSBatteryGood)
  {
    DisableCharge();
    BatteryStatus &= ~((uint8_t)PiUPSBatteryChg);
    
    EnableBatt();
    BatteryStatus |= PiUSPBatteryEn;
  }
  
  // No power to provide condition:
  else
  {
    DisableCharge();
    BatteryStatus &= ~((uint8_t)PiUPSBatteryChg);
    
    DisableBatt();
    BatteryStatus &= ~((uint8_t)PiUSPBatteryEn);
  }
}
 
void UpdateStatus(void)
{
  UpdateBatteryStatus();
  UpdateBatteryState();

  
  // Determine the rail source voltage:
  if (voltage_aux2 > voltage_aux1)
  {
    // Check battery output is on and powering rail:
    if ((BatteryStatus & PiUSPBatteryEn) && (voltage_aux2 < voltage_rail))
    {
      RailSource = PiUPSPowerBatt;
    }
    else
    {
      RailSource = PiUPSPowerAUX2;
    }
  }
  else
  {
    // Check battery output is on and powering rail:
    if ((BatteryStatus & PiUSPBatteryEn) && (voltage_aux2 < voltage_rail))
    {
      RailSource = PiUPSPowerBatt;
    }
    else
    {
      RailSource = PiUPSPowerAUX1;
    }
  
  }
  
  UpdateBatteryState();
  
  // If voltage rail is above the first power regulator
  // then the device is being powered from the rail, and
  // is powered from the highest supply:
  if ( voltage_rail > voltage_bat )
  {
    PowerSource = RailSource;
  }
  else
  {
    RailSource = PiUPSPowerBatt;
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
    TIMSK |= (1<<TOIE1); // Interrupt on overflow
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
      case PiUPSStatus:
        temp = BatteryStatus | 
               (PowerSource << 4) |
               (RailSource  << 8);
        break;
               
      case PiUPSADCStatus:
        temp = CurrentADCState;
        break;
        
      // Voltage Reading:
      case PiUPSVcc:
        temp = voltage_vcc;
        break;
        
      case PiUPSVBatt:
        temp = voltage_bat;
        break;
        
      case PiUPSVRail:
        temp = voltage_rail;
        break;
        
      case PiUPSVAux1:
        temp = voltage_aux1;
        break;
        
      case PiUPSVAux2:
        temp = voltage_aux2;
        break;
        
      case PiUPSV5V:
        temp = voltage_5v;
        break;
        
      case PiUPSVAuxO:
        temp = voltage_aout;
        break;
        
      case PiUPSIRail:
        temp = current_rail;
        break;
      
      // Conversion Factors:
      case PiUPSVBattConv:
        temp = read_eeprom_word (EEPROM_VBATT_CONV);
        break;
        
      case PiUPSVRailConv:
        temp = read_eeprom_word (EEPROM_VRAIL_CONV);
        break;
        
      case PiUPSVAux1Conv:
        temp = read_eeprom_word (EEPROM_VAUX1_CONV);
        break;
        
      case PiUPSVAux2Conv:
        temp = read_eeprom_word (EEPROM_VAUX2_CONV);
        break;
        
      case PiUPSV5VConv:
        temp = read_eeprom_word (EEPROM_V5V_CONV);
        break;
        
      case PiUPSVAuxOConv:
        temp = read_eeprom_word (EEPROM_VAUXO_CONV);
        break;
      
      case PiUPSIRailConv:
        temp = read_eeprom_word (EEPROM_IRAIL_CONV);
        break;
        
      case PiUPSVBattLowDis:
        temp = read_eeprom_word (EEPROM_VBATT_LOWDIS);
        break;
        
      case PiUPSVBattLowEn:
        temp = read_eeprom_word (EEPROM_VBATT_LOWEN);
        break;
          
      case PiUPSRailLowSw:
        temp = read_eeprom_word (EEPROM_RAIL_LOWSW);
        break;
        
      case PiUPSRailHighSw:
        temp = read_eeprom_word (EEPROM_RAIL_HIGHSW);
        break;
        
      case PiUPSRailCompEn:
        temp = read_eeprom_word (EEPROM_RAIL_COMPEN);
        break;
        
      case PiUPSIRailLim:
        temp = read_eeprom_word (EEPROM_IRAILLIM);
        break;
        
      case PiUPSChargeExcess:
        temp = read_eeprom_word (EEPROM_CHGEXCESS);
        break;
       
        
    }
    
    output_buffer[0] = (temp >> 8);
    output_buffer[1] = (temp);
    
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
        
      case PiUPSVBattConv:
        update_eeprom_byte (EEPROM_VBATT_CONV+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_VBATT_CONV, input_buffer[2]);
        break;
      
      case PiUPSVRailConv:
        update_eeprom_byte (EEPROM_VRAIL_CONV+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_VRAIL_CONV, input_buffer[2]);
        break;
        
      case PiUPSVAux1Conv:
        update_eeprom_byte (EEPROM_VAUX1_CONV+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_VAUX1_CONV, input_buffer[2]);
        break;
        
      case PiUPSVAux2Conv:
        update_eeprom_byte (EEPROM_VAUX2_CONV+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_VAUX2_CONV, input_buffer[2]);
        break;
        
      case PiUPSV5VConv:
        update_eeprom_byte (EEPROM_V5V_CONV+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_V5V_CONV, input_buffer[2]);
        break;
        
      case PiUPSVAuxOConv:
        update_eeprom_byte (EEPROM_VAUXO_CONV+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_VAUXO_CONV, input_buffer[2]);
        break;
      
      case PiUPSIRailConv:
        update_eeprom_byte (EEPROM_IRAIL_CONV+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_IRAIL_CONV, input_buffer[2]);
        break;
        
      case PiUPSVBattLowDis:
        update_eeprom_byte (EEPROM_VBATT_LOWDIS+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_VBATT_LOWDIS, input_buffer[2]);
        break;
      
      case PiUPSVBattLowEn:
        update_eeprom_byte (EEPROM_VBATT_LOWEN+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_VBATT_LOWEN, input_buffer[2]);
        break;
            
      case PiUPSRailLowSw:
        update_eeprom_byte (EEPROM_RAIL_LOWSW+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_RAIL_LOWSW, input_buffer[2]);
        break;
        
      case PiUPSRailHighSw:
        update_eeprom_byte (EEPROM_RAIL_HIGHSW+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_RAIL_HIGHSW, input_buffer[2]);
        break;
        
      case PiUPSRailCompEn:
        update_eeprom_byte (EEPROM_RAIL_COMPEN+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_RAIL_COMPEN, input_buffer[2]);
        break;
        
      case PiUPSIRailLim:
        update_eeprom_byte (EEPROM_IRAILLIM+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_IRAILLIM, input_buffer[2]);
        break;
        
      case PiUPSChargeExcess:
        update_eeprom_byte (EEPROM_CHGEXCESS+1, input_buffer[1]);
        update_eeprom_byte (EEPROM_CHGEXCESS, input_buffer[2]);
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
    /*if ((ACSR & (1<<ACO)) == 0)
    {
        PORTB &= ~(1 << PB0); // PD0 goes low
    }*/

    ADCStateHandler ();
}

/*****************************************************************************
 * Comparator operation
 *****************************************************************************/
/*
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
        RailSource = PiUPSPowerBatt;
    }
}
*/

