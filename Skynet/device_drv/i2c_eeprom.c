//  ***************************************************************************
/// @file    i2c_eeprom.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <stdbool.h>
#include <string.h>
#include "i2c.h"
#include "i2c_eeprom.h"
#include "systimer.h"
#define I2C_EEPROM_ADDRESS			(0x50)
#define MAX_BLOCK_SIZE				(7)


//  ***************************************************************************
/// @brief  Function for read data from I2C EEPROM
/// @param	ee_address: address to EEPROM memory cell
/// @param	data: buffer address
/// @param	bytes_count: bytes count for read
/// @return	true - read success, false - fail
//  ***************************************************************************
bool i2c_eeprom_read_bytes(uint32_t ee_address, uint8_t* data, uint32_t bytes_count) {

	while (bytes_count != 0) {

		// Make block size
		uint32_t block_size = 0;
		if (bytes_count < MAX_BLOCK_SIZE) {
			block_size = bytes_count;
		}
		else {
			block_size = MAX_BLOCK_SIZE;
		}

		// Read block from EEPROM
		if (i2c_read_bytes(I2C_EEPROM_ADDRESS, ee_address, 2, data, block_size) == false) {
			return false;
		}

		// Offset
		ee_address += block_size;
		data += block_size;
		bytes_count -= block_size;
	}
	return true;
}

//  ***************************************************************************
/// @brief	Function for write data to I2C EEPROM
/// @param	ee_address: address to EEPROM memory cell
/// @param	data: buffer address
/// @param	bytes_count: bytes count for write
/// @return	true - write success, false - fail
//  ***************************************************************************
bool eeprom_write_bytes(uint32_t ee_address, uint8_t* data, uint32_t bytes_count) {

	while (bytes_count != 0) {

		// Make block size
		uint32_t block_size = 0;
		if (bytes_count < MAX_BLOCK_SIZE) {
			block_size = bytes_count;
		}
		else {
			block_size = MAX_BLOCK_SIZE;
		}
		
		// Write block to EEPROM
		if (i2c_write_bytes(I2C_EEPROM_ADDRESS, ee_address, 2, data, block_size) == false) {
			return false;
		}

		// Offset
		ee_address += block_size;
		data += block_size;
		bytes_count -= block_size;

		delay_ms(50);
	}
	return true;
}

//  ***************************************************************************
/// @brief	Function for write 4 bytes (number) to I2C EEPROM
/// @param	ee_address: address to EEPROM memory cell
/// @param	data: number for write
/// @param	bytes_count: bytes count for write
/// @return	true - write success, false - fail
//  ***************************************************************************
bool i2c_eeprom_write_4bytes(uint32_t ee_address, uint32_t data, uint32_t bytes_count) {
	
	return eeprom_write_bytes(ee_address, (uint8_t*)&data, bytes_count);
}