#ifndef PiUPSDefines_h
#define PiUPSDefines_h



typedef enum PiUPSBattery
{
    PiUPSBatteryLow = 0x1,
    PiUPSBatteryChg = 0x2,
} PiUPSBattery;


typedef enum PiUPSPower
{
    PiUPSPowerBatt = 0x1,
    PiUPSPowerAUX1 = 0x2,
    PiUPSPowerAUX2 = 0x4
} PiUPSPower;

// ADC State Machine
typedef enum ADCState
{
    ADCIdle = 0x0,
    ADCInit = 0x1,
    ADCInitWait = 0x2,
    ADCVccWait = 0x3,
    ADCVbatWait = 0x4,
    ADCVrailWait = 0x5,
    ADCVauxlWait = 0x6,
    ADCConvComplete = 0xF0,
    ADCDisable = 0xFC,
    ADCERR = 0xFF
} ADCState;


// Write commands (opcodes)
#define PIUPS_RESET 0x0
#define PIUPS_UPDATE 0x1
#define PIUPS_SAVE 0x2
#define PIUPS_SETI2C 0x13

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

#define PIUPS_VBATT_LOWDIS 0x60
#define PIUPS_VBATT_LOWEN 0x62





#endif

