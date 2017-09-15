#ifndef eeprom_h
#define eeprom_h

//Some helper macros
#define read_eeprom_byte(address) eeprom_read_byte ((const void *)address)
#define write_eeprom_byte(address,value_p) eeprom_write_byte ((void *)address, value_p)
#define update_eeprom_byte(address,value_p) eeprom_update_byte ((void *)address, value_p)

#define read_eeprom_word(address) eeprom_read_word ((const void *)address)
#define write_eeprom_word(address,value_p) eeprom_write_word ((void *)address, value_p)
#define update_eeprom_word(address,value_p) eeprom_update_word ((void *)address, value_p)

//#define read_eeprom_array(address,value_p,length) eeprom_read_block ((void *)value_p, (const void *)address, length)
//#define write_eeprom_array(address,value_p,length) eeprom_write_block ((const void *)value_p, (void *)address, length)
//#define update_eeprom_array(address,value_p,length) eeprom_update_block ((const void *)value_p, (void *)address, length)

// Byte of memory to store infomation in:
#define EEPROM_UUID 0x01
#define EEPROM_I2CADDR 0x02

// Place to store hardware configuration

#define EEPROM_VBATT_CONV 0x10
#define EEPROM_VRAIL_CONV 0x12
#define EEPROM_VAUX1_CONV 0x14

// Battery low safety points:
#define EEPROM_VBATT_LOWDIS 0x20
#define EEPROM_VBATT_LOWEN 0x22




#endif
