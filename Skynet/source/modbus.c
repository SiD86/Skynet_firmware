//  ***************************************************************************
/// @file    modbus.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <string.h>
#include "modbus.h"
#include "usart1_pdc.h"
#include "ram_map.h"
#include "veeprom.h"

#define CRC16_POLYNOM							(0xA001)	// CRC polynom

#define MB_MIN_REQUEST_SIZE						(7)
#define MB_READ_RAM_CMD_MIN_LENGTH				(7)
#define MB_WRITE_RAM_CMD_MIN_LENGTH				(8)
#define MB_READ_EEPROM_CMD_MIN_LENGTH			(7)
#define MB_WRITE_EEPROM_CMD_MIN_LENGTH			(8)

#define MAX_READ_RAM_SIZE						(16)
#define MAX_WRITE_RAM_SIZE						(16)
#define MAX_READ_EEPROM_SIZE					(16)
#define MAX_WRITE_EEPROM_SIZE					(16)

#define MB_CMD_WRITE_RAM						(0x41)		// ModBus Function Code: Write RAM
#define MB_CMD_WRITE_EEPROM						(0x43)		// ModBus Function Code: Write EEPROM
#define MB_CMD_READ_RAM							(0x44)		// ModBus Function Code: Read RAM
#define MB_CMD_READ_EEPROM						(0x46)		// ModBus Function Code: Read EEPROM

#define MB_OK									(0x00)
#define MB_EXCEPTION_ILLEGAL_FUNCTION			(0x01)		// ModBus Exception code: Illegal Function. Requested Function is not supported, or is not supported in current Device mode.
#define MB_EXCEPTION_ILLEGAL_DATA_ADDRESS		(0x02)		// ModBus Exception code: Illegal Data Address. Requested Address is out of range.
#define MB_EXCEPTION_ILLEGAL_DATA_VALUE			(0x03)		// ModBus Exception code: Illegal Data Value. Requested amount of Data is out of range.
#define MB_EXCEPTION_SLAVE_DEV_FAILURE			(0x04)		// ModBus Exception code: Slave Device Failure. Device can't execute incoming command (EEPROM failure, insufficient RAM, etc).
#define MB_BAD_FRAME							(0xFF)


static bool is_request_valid(const uint8_t* request, uint32_t size);
uint32_t read_ram_command_handler(const uint8_t* request, uint8_t* response, uint16_t rq_size, uint8_t* rs_size);
uint32_t write_ram_command_handler(const uint8_t* request, uint16_t rq_size);
static uint32_t read_eeprom_command_handler(const uint8_t* request, uint8_t* response, uint8_t rq_size, uint8_t* rs_size);
uint32_t write_eeprom_command_handler(const uint8_t* request, uint16_t rq_size);
static uint16_t calculate_crc16(const uint8_t* data, uint32_t size);


//  ***************************************************************************
/// @brief  Modbus initialization
/// @note	none
/// @param  none
/// @return none
//  ***************************************************************************
void modbus_init(void) {
	
	usart1_init();
	usart1_start_rx();
}

//  ***************************************************************************
/// @brief  Modbus process
/// @note	Call from main loop
/// @param  none
/// @return none
//  ***************************************************************************
void modbus_process(void) {
	
	// Check USART errors
	if (usart1_is_error() == true) {
		usart1_reset(true, true);
		usart1_start_rx();
		return;
	}
	
	// Check frame receive
	if (usart1_is_frame_receive() == false) {
		return;
	}
	
	// Check complete transmit response
	if (usart1_is_tx_complete() == false) {
		usart1_start_rx();
		return;
	}
	

	uint8_t* response = usart1_get_tx_buffer_address();
	uint8_t* request = usart1_get_rx_buffer_address();
	uint32_t request_size = usart1_get_frame_size();
	
	// Verify frame
	if (is_request_valid(request, request_size) == false) {
		usart1_start_rx();
		return;
	}
	
	// Process command
	uint8_t response_size = 0;
	uint32_t result = MB_OK;
	
	switch (request[1]) {
		
		case MB_CMD_READ_RAM:
			result = read_ram_command_handler(request, response, request_size, &response_size);
			break;

		case MB_CMD_WRITE_RAM:
			result = write_ram_command_handler(request, request_size);
			break;
		
		case MB_CMD_READ_EEPROM:
			result = read_eeprom_command_handler(request, response, request_size, &response_size);
			break;

		case MB_CMD_WRITE_EEPROM:
			result = write_eeprom_command_handler(request, request_size);
			break;

		default:
			usart1_start_rx();
			return;
	}
	
	// Check result
	if (result != MB_OK) {
		
		if (result != MB_BAD_FRAME) {
			
			// Make and send exception
			response[1] = request[0] | 0x80;
			response[2] = result;
			
			uint16_t crc = calculate_crc16(response, response_size);
			response[response_size++] = crc & 0xFF;
			response[response_size++] = crc >> 8;
			
			usart1_start_tx(response_size);
			usart1_start_rx();
		}
		return;
	}
	
	// Make response
	response[0] = request[0];
	response[1] = request[1];
	response_size += 2;
	
	uint16_t crc = calculate_crc16(response, response_size);
	response[response_size++] = crc & 0xFF;
	response[response_size++] = crc >> 8;
	
	// Send response
	usart1_start_tx(response_size);
	usart1_start_rx();
}




//  ***************************************************************************
/// @brief  Check modbus frame
/// @param  request: modbus request
/// @param  size:  frame size
/// @return true - frame valid, false - frame invalid
//  ***************************************************************************
static bool is_request_valid(const uint8_t* request, uint32_t size) {
	
	// Check frame size
	if (size < MB_MIN_REQUEST_SIZE) {
		return false;
	}
	
	// Check device address
	if (request[0] != 0xFE && request[0] != 0x68) {
		return false;
	}
	
	// Check CRC
	uint16_t crc = calculate_crc16(request, size);
	if (crc != 0) {
		return false;
	}
	
	return true;
}

//  ***************************************************************************
/// @brief  Function for processing Modbus read RAM command
/// @param  request: modbus request
/// @param  response modbus response
/// @param  rq_size  request size
/// @param  rs_size  response size
/// @retval response
/// @retval rs_size
/// @return command process result
//  ***************************************************************************
uint32_t read_ram_command_handler(const uint8_t* request, uint8_t* response, uint16_t rq_size, uint8_t* rs_size) {
	
	// Check request size
	if (rq_size < MB_READ_RAM_CMD_MIN_LENGTH) {
		return MB_BAD_FRAME;
	}
	
	// Parse request parameters
	uint16_t address = (request[2] << 8) | request[3];
	uint8_t bytes_count = request[4];
	if (bytes_count == 0 || bytes_count > MAX_READ_RAM_SIZE) {
		return MB_EXCEPTION_ILLEGAL_DATA_VALUE;
	}

	// Check request parameters
	if (address + bytes_count - 1 > RAM_MAP_END_ADDRESS) {
		return MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;
	}
	
	// Process command
	ram_map_get_data(address, &response[3], bytes_count);
	
	response[2] = bytes_count;
	*rs_size += bytes_count + 1; // +1: response[2] = bytes_count;
	
	return MB_OK;
}

//  ***************************************************************************
/// @brief  Function for processing Modbus write RAM command
/// @param  request: modbus request
/// @param  response modbus response
/// @param  rq_size  request size
/// @retval response
/// @return command process result
//  ***************************************************************************
uint32_t write_ram_command_handler(const uint8_t* request, uint16_t rq_size) {
	
	// Check request size
	if (rq_size < MB_WRITE_RAM_CMD_MIN_LENGTH) {
		return MB_BAD_FRAME;
	}
	
	// Parse request parameters
	uint16_t address = (request[2] << 8) | request[3];
	uint8_t bytes_count = request[4];
	if (bytes_count == 0 || bytes_count > MAX_READ_RAM_SIZE) {
		return MB_EXCEPTION_ILLEGAL_DATA_VALUE;
	}

	// Check request parameters
	if (address + bytes_count - 1 > RAM_MAP_END_ADDRESS) {
		return MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;
	}

	// Process command
	ram_map_put_data(address, &request[5], bytes_count);
	
	return MB_OK;
}

//  ***************************************************************************
/// @brief  Function for processing modbus read EEPROM command
/// @param  request: modbus request
/// @param  response modbus response
/// @param  rq_size  request size
/// @param  rs_size  response size
/// @retval response
/// @retval rs_size
/// @return command process result
//  ***************************************************************************
static uint32_t read_eeprom_command_handler(const uint8_t* request, uint8_t* response, uint8_t rq_size, uint8_t* rs_size) {
	
	// Check request size
	if (rq_size < MB_READ_EEPROM_CMD_MIN_LENGTH) {
		return MB_BAD_FRAME;
	}
	
	// Parse request parameters
	uint16_t address = (request[2] << 8) | request[3];
	uint8_t bytes_count = request[4];
	if (bytes_count == 0 || bytes_count > MAX_READ_EEPROM_SIZE) {
		return MB_EXCEPTION_ILLEGAL_DATA_VALUE;
	}

	// Check request parameters
	/*if (address + bytes_count - 1 > INTERNAL_RAM_END_ADDRESS) {
		return MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;
	}*/
	
	// Process command
	response[2] = bytes_count;
	veeprom_read_bytes(address, &response[3], bytes_count);
	*rs_size += bytes_count + 1; // +1: response[2] = bytes_count;
	
	return MB_OK;
}

//  ***************************************************************************
/// @brief  Function for processing modbus write EEPROM command
/// @param  request: modbus request
/// @param  response modbus response
/// @param  rq_size  request size
/// @retval response
/// @return command process result
//  ***************************************************************************
uint32_t write_eeprom_command_handler(const uint8_t* request, uint16_t rq_size) {
	
	// Check request size
	if (rq_size < MB_WRITE_EEPROM_CMD_MIN_LENGTH) {
		return MB_BAD_FRAME;
	}
	
	// Parse request parameters
	uint16_t address = (request[2] << 8) | request[3];
	uint8_t bytes_count = request[4];
	if (bytes_count == 0 || bytes_count > MAX_READ_EEPROM_SIZE) {
		return MB_EXCEPTION_ILLEGAL_DATA_VALUE;
	}
	
	// Check request parameters
	/*if (address + bytes_count - 1 > INTERNAL_RAM_END_ADDRESS) {
		return MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;
	}*/
	
	// Process command
	veeprom_write_bytes(address, &request[5], bytes_count);
	
	return MB_OK;
}

//  ***************************************************************************
/// @brief  Calculate modbus frame CRC16
/// @param  frame: modbus frame
/// @param  size:  frame size
/// @return CRC16 value
//  ***************************************************************************
static uint16_t calculate_crc16(const uint8_t* frame, uint32_t size) {
	
	uint16_t crc16 = 0xFFFF;
	uint16_t data = 0;
	uint16_t k = 0;
	
	while (size--) {
		crc16 ^= *frame++;
		k = 8;
		while (k--) {
			data = crc16;
			crc16 >>= 1;
			if (data & 0x0001) {
				crc16 ^= CRC16_POLYNOM;
			}
		}
	}
	return crc16;
}