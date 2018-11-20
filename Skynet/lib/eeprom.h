//  ***************************************************************************
/// @file    eeprom.h
/// @author  NeoProg
/// @brief   EEPROM driver
//  ***************************************************************************
#ifndef EEPROM_H_
#define EEPROM_H_


bool eeprom_read_bytes(uint32_t address, uint8_t* data, uint32_t size);
bool eeprom_write_bytes(uint32_t address, uint8_t* data, uint32_t size);
bool eeprom_write_4bytes(uint32_t address, uint32_t data, uint32_t size);


#endif /* __EEPROM_H__ */