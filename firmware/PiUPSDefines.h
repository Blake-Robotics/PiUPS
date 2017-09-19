#ifndef PiUPSDefines_h
#define PiUPSDefines_h



typedef enum PiUPSBattery
{
    PiUPSBatteryLow  = 0x1,
    PiUPSBatteryGood = 0x2,
    PiUPSBatteryChg  = 0x4,
} PiUPSBattery;


typedef enum PiUPSPower
{
    PiUPSPowerBatt = 0x1,
    PiUPSPowerAUX1 = 0x2,
    PiUPSPowerAUX2 = 0x4
} PiUPSPower;

// ADC State Machine
typedef enum PiUPSADCState
{
    ADCIdle = 0x0,
    ADCInit = 0x1,
    ADCInitWait = 0x2,
    ADCVccWait = 0x3,
    ADCVbatWait = 0x4,
    ADCVrailWait = 0x5,
    ADCVauxlWait = 0x6,
    ADCVaux2Wait = 0x7,
    ADCConvComplete = 0xF0,
    ADCDisable = 0xFC,
    ADCERR = 0xFF
} PiUPSADCState;


// Write commands (opcodes)
//#define PIUPS_RESET 0x0
//#define PIUPS_UPDATE 0x1
//#define PIUPS_SAVE 0x2
//#define PIUPS_SETI2C 0x13

//Status and control:
//  [3:0] - PiUPSBattery: Battery Status
//  [7:4] - PiUPSPower: Rail power source
//  [11:8] - PiUPSPower: Power supply
#define PIUPS_STATUS 0x10
#define PIUPS_ADCSTATE 0x18

// Voltages to read.
#define PIUPS_VCC 0x20
#define PIUPS_VBATT 0x22
#define PIUPS_VRAIL 0x24
#define PIUPS_VAUX1 0x26
#define PIUPS_VAUX2 0x28

// Conversion factors:
#define PIUPS_VBATT_CONV 0x50
#define PIUPS_VRAIL_CONV 0x52
#define PIUPS_VAUX1_CONV 0x54
#define PIUPS_VAUX2_CONV 0x56

// Voltages for constraining safe
// battery voltages:
#define PIUPS_VBATT_LOWDIS 0x60
#define PIUPS_VBATT_LOWEN 0x62

// Range for operating rail voltages
#define PIUPS_RAIL_LOWSW 0x64
#define PIUPS_RAIL_HIGHSW 0x66
#define PIUPS_RAIL_COMPEN 0x68

//TODO: Add charging support using DC_DC converter..



#endif

