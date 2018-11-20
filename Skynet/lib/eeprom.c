//  ***************************************************************************
/// @file    eeprom.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <stdbool.h>
#include <string.h>
#include "i2c.h"
#include "eeprom.h"
#include "systimer.h"
#define EEPROM_I2C_ADDRESS			(0x50)
#define MAX_BLOCK_SIZE				(7)


//  ***************************************************************************
/// @brief  Function for read data from EEPROM
/// @param	address: address to EEPROM memory cell
/// @param	data: buffer address
/// @param	size: count bytes for read
/// @return	true - read success, false - fail
//  ***************************************************************************
bool eeprom_read_bytes(uint32_t address, uint8_t* data, uint32_t size) {

	i2c_set_internal_address_length(2);

	while (size != 0) {

		// Make block size
		uint32_t count = 0;
		if (size < MAX_BLOCK_SIZE) {
			count = size;
		}
		else {
			count = MAX_BLOCK_SIZE;
		}

		// Read block from EEPROM
		if (i2c_read_bytes(EEPROM_I2C_ADDRESS, address, data, count) == false)
			return false;

		// Offset
		address += count;
		data += count;
		size -= count;
	}
	return true;
}

//  ***************************************************************************
/// @brief	Function for write data to EEPROM
/// @param	address: address to EEPROM memory cell
/// @param	data: buffer address
/// @param	size: count bytes for write
/// @return	true - write success, false - fail
//  ***************************************************************************
bool eeprom_write_bytes(uint32_t address, uint8_t* data, uint32_t size) {

	i2c_set_internal_address_length(2);

	while (size != 0) {

		// Make block size
		uint32_t count = 0;
		if (size < MAX_BLOCK_SIZE) {
			count = size;
		}
		else {
			count = MAX_BLOCK_SIZE;
		}
		
		// Write block to EEPROM
		if (i2c_write_bytes(EEPROM_I2C_ADDRESS, address, data, count) == false)
			return false;

		// Offset
		address += count;
		data += count;
		size -= count;

		delay_ms(50);
	}
	return true;
}

//  ***************************************************************************
/// @brief	Function for write 4 bytes (number) to EEPROM
/// @param	address: address to EEPROM memory cell
/// @param	data: number for write
/// @param	size: count bytes for write
/// @return	true - write success, false - fail
//  ***************************************************************************
bool eeprom_write_4bytes(uint32_t address, uint32_t data, uint32_t size) {
	
	return eeprom_write_bytes(address, (uint8_t*)&data, size);
}