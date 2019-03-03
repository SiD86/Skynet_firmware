//  ***************************************************************************
/// @file    i2c_eeprom.h
/// @author  NeoProg
/// @brief   EEPROM driver
//  ***************************************************************************
#ifndef I2C_EEPROM_H_
#define I2C_EEPROM_H_


bool i2c_eeprom_read_bytes(uint32_t ee_address, uint8_t* data, uint32_t bytes_count);
bool i2c_eeprom_write_bytes(uint32_t ee_address, uint8_t* data, uint32_t bytes_count);
bool i2c_eeprom_write_4bytes(uint32_t ee_address, uint32_t data, uint32_t bytes_count);


#endif /* I2C_EEPROM_H_ */