//  ***************************************************************************
/// @file    veeprom.h
/// @author  NeoProg
/// @brief   Virtual EEPROM driver
//  ***************************************************************************
#ifndef VEEPROM_H_
#define VEEPROM_H_

#include <stdint.h>
#include <stdbool.h>


void veeprom_init(void);
bool veeprom_update_checksum(void);

uint8_t  veeprom_read_8(uint32_t veeprom_address);
uint16_t veeprom_read_16(uint32_t veeprom_address);
uint32_t veeprom_read_32(uint32_t veeprom_address);
void     veeprom_read_bytes(uint32_t veeprom_address, uint8_t* data, uint32_t size);

bool veeprom_write_8(uint32_t veeprom_address, uint8_t data);
bool veeprom_write_16(uint32_t veeprom_address, uint16_t data);
bool veeprom_write_32(uint32_t veeprom_address, uint32_t data);
bool veeprom_write_bytes(uint32_t veeprom_address, const uint8_t* data, uint32_t size);


#endif /* __VEEPROM_H__ */