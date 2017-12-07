#ifndef PiUPSDefines_h
#define PiUPSDefines_h



typedef enum PiUPSBattery
{
    PiUPSBatteryLow  = 0x1,
    PiUPSBatteryGood = 0x2,
    PiUPSBatteryChg  = 0x4,
    PiUSPBatteryEn   = 0x8,
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
    ADCVbatInit = 0x4,
    ADCVbatWait = 0x5,
    ADCVrailWait = 0x6,
    ADCVauxlWait = 0x7,
    ADCVaux2Wait = 0x8,
    ADCV5VWait = 0x9,
    ADCVauxOWait = 0x10,
    ADCVauxCurrWait = 0x11,
    ADCConvComplete = 0xF0,
    ADCDisable = 0xFC,
    ADCERR = 0xFF
} PiUPSADCState;

/// PiUPS Register map
typedef enum PiUPSRegister
{
    // 0x00 - 0x0F: I2C config registers:
    PiUPSReset = 0x0, ///< Reset (unused)
    PiUPSUpdate =  0x1, ///< Update (unused)
    PiUPSSave = 0x2, ///< Update (unused)
    PiUPSSetI2C = 0x5,   ///< Set I2C addr (unused)

    // 0x10 - 0x1F: System status/control registers
    PiUPSStatus = 0x10,    ///< Current Status of the PiUPS system
    PiUPSADCStatus = 0x18, ///< Current State of the ADC state machine

    // 0x20 - 0x2F: Sensor status registers
    PiUPSVcc = 0x20, ///< Vcc Voltage (mv)
    PiUPSVBatt = 0x22, ///< Battery Voltage (mv)
    PiUPSVRail = 0x24, ///< Rail Voltage (mv)
    PiUPSVAux1 = 0x26, ///< Aux1 Voltage (mv)
    PiUPSVAux2 = 0x28, ///< Aux2 Voltage (mv)
    PiUPSV5V   = 0x2A, ///< 5V Output Voltage (mv)
    PiUPSVAuxO = 0x2C, ///< AuxO Output Voltage (mv)
    PiUPSIRail = 0x2E, ///< Rail Current (mA)
    
    // 0x50 - 0x5F Sensor conversion factors
    PiUPSVBattConv = 0x50, ///< Conversion factor for VBatt * 1000
    PiUPSVRailConv = 0x52, ///< Conversion factor for VRail * 1000
    PiUPSVAux1Conv = 0x54, ///< Conversion factor for VAux1 * 1000
    PiUPSVAux2Conv = 0x56, ///< Conversion fator for VAux2 * 1000
    PiUPSV5VConv = 0x58,   ///< Conversion factor for V5V * 1000
    PiUPSVAuxOConv = 0x5A, ///< Conversion factor for AuxO output * 1000
    PiUPSIRailConv = 0x5C, ///< Conversion factor for Rail current * 1000

    // 0x60 - Condiguration for safe switching voltages
    PiUPSVBattLowDis = 0x60, ///< Disable battery use and switch MCU into powersave once this voltage is reached
    PiUPSVBattLowEn = 0x62,  ///< Enable battery use and exit powersave once this voltage is reached

    // Range for operating rail voltages
    PiUPSRailLowSw = 0x64,
    PiUPSRailHighSw = 0x66,
    PiUPSRailCompEn = 0x68,
    
    PiUPSIRailLim = 0x6A,    ///< Disable AuxO/Battery charging if the current here is exceeded
    PiUPSChargeExcess = 0x6C, ///< Excess voltage needed to enable the battery charging circuit
    PiUPSChargeLimit = 0x6E   ///< Maximum voltage the battery should be charged to
} PiUPSRegister;


// Write commands (opcodes)
//#define PIUPS_RESET 0x0
//#define PIUPS_UPDATE 0x1
//#define PIUPS_SAVE 0x2
//#define PIUPS_SETI2C 0x13

//Status and control:
// TODO: Fix data widths..
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
#define PIUPS_V5V   0x2A
#define PIUPS_VAUXO 0x2C
#define PIUPS_IRAIL 0x2E

// Conversion factors:
#define PIUPS_VBATT_CONV 0x50
#define PIUPS_VRAIL_CONV 0x52
#define PIUPS_VAUX1_CONV 0x54
#define PIUPS_VAUX2_CONV 0x56
#define PIUPS_V5V_CONV 0x58
#define PIUPS_VAUXO_CONV 0x5A
#define PIUPS_IRAIL_CONV 0x5C

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

