//  ***************************************************************************
/// @file    ram_map.c
/// @author  NeoProg
//  ***************************************************************************
#include "ram_map.h"

#include <sam.h>
#include <stdlib.h>
#include "servo_driver.h"
#include "scr.h"
#include "error_handling.h"
        
#define RAM_PUT_BYTE(ram_addr, var)  .data[ram_addr] = (uint8_t*)&(var)

#define RAM_PUT_WORD(ram_addr, var)  .data[(ram_addr) + 0] = ((uint8_t*)&(var)) + 1,	\
                                     .data[(ram_addr) + 1] = ((uint8_t*)&(var)) + 0

#define RAM_PUT_DWORD(ram_addr, var) .data[(ram_addr) + 0] = ((uint8_t*)&(var)) + 3,	\
                                     .data[(ram_addr) + 1] = ((uint8_t*)&(var)) + 2,	\
                                     .data[(ram_addr) + 2] = ((uint8_t*)&(var)) + 1,	\
                                     .data[(ram_addr) + 3] = ((uint8_t*)&(var)) + 0


extern int16_t x;
extern int16_t y;
extern int16_t z;

typedef struct {
    uint8_t* data[RAM_MAP_SIZE];
} ram_map_t;

static const ram_map_t ram_map = {
    
	RAM_PUT_DWORD(0x0000, error_status),
	
	RAM_PUT_WORD (0x0040, x),
	RAM_PUT_WORD (0x0042, y),
	RAM_PUT_WORD (0x0044, z),
	
	RAM_PUT_BYTE (0x0060, scr),
	
	RAM_PUT_BYTE (0x00C0, ram_servo_angle[0]),
	RAM_PUT_BYTE (0x00C1, ram_servo_angle[1]),
	RAM_PUT_BYTE (0x00C2, ram_servo_angle[2]),
	RAM_PUT_BYTE (0x00C3, ram_servo_angle[3]),
	RAM_PUT_BYTE (0x00C4, ram_servo_angle[4]),
	RAM_PUT_BYTE (0x00C5, ram_servo_angle[5]),
	RAM_PUT_BYTE (0x00C6, ram_servo_angle[6]),
	RAM_PUT_BYTE (0x00C7, ram_servo_angle[7]),
	RAM_PUT_BYTE (0x00C8, ram_servo_angle[8]),
	RAM_PUT_BYTE (0x00C9, ram_servo_angle[9]),
	RAM_PUT_BYTE (0x00CA, ram_servo_angle[10]),
	RAM_PUT_BYTE (0x00CB, ram_servo_angle[11]),
	RAM_PUT_BYTE (0x00CC, ram_servo_angle[12]),
	RAM_PUT_BYTE (0x00CD, ram_servo_angle[13]),
	RAM_PUT_BYTE (0x00CE, ram_servo_angle[14]),
	RAM_PUT_BYTE (0x00CF, ram_servo_angle[15]),
	RAM_PUT_BYTE (0x00D0, ram_servo_angle[16]),
	RAM_PUT_BYTE (0x00D1, ram_servo_angle[17]),
	
	RAM_PUT_BYTE (0x00E0, ram_servo_angle_override[0]),
	RAM_PUT_BYTE (0x00E1, ram_servo_angle_override[1]),
	RAM_PUT_BYTE (0x00E2, ram_servo_angle_override[2]),
	RAM_PUT_BYTE (0x00E3, ram_servo_angle_override[3]),
	RAM_PUT_BYTE (0x00E4, ram_servo_angle_override[4]),
	RAM_PUT_BYTE (0x00E5, ram_servo_angle_override[5]),
	RAM_PUT_BYTE (0x00E6, ram_servo_angle_override[6]),
	RAM_PUT_BYTE (0x00E7, ram_servo_angle_override[7]),
	RAM_PUT_BYTE (0x00E8, ram_servo_angle_override[8]),
	RAM_PUT_BYTE (0x00E9, ram_servo_angle_override[9]),
	RAM_PUT_BYTE (0x00EA, ram_servo_angle_override[10]),
	RAM_PUT_BYTE (0x00EB, ram_servo_angle_override[11]),
	RAM_PUT_BYTE (0x00EC, ram_servo_angle_override[12]),
	RAM_PUT_BYTE (0x00ED, ram_servo_angle_override[13]),
	RAM_PUT_BYTE (0x00EE, ram_servo_angle_override[14]),
	RAM_PUT_BYTE (0x00EF, ram_servo_angle_override[15]),
	RAM_PUT_BYTE (0x00F0, ram_servo_angle_override[16]),
	RAM_PUT_BYTE (0x00F1, ram_servo_angle_override[17])
};


//  ***************************************************************************
/// @brief	Read RAM to external buffer
/// @param	ram_address: RAM address
/// @param	buffer: pointer to buffer
/// @param	bytes_count: bytes count for read
/// @return	true - read data success, false - error
//  ***************************************************************************
bool ram_map_read(uint32_t ram_address, uint8_t* buffer, uint32_t bytes_count) {
	
	if (ram_address + bytes_count >= RAM_MAP_SIZE) {
		return false;	
	}
    
    for (uint32_t i = 0; i < bytes_count; ++i) {
        buffer[i] = (ram_map.data[ram_address + i] != NULL) ? *(ram_map.data[ram_address + i]) : 0;
    }
	
	return true;
}

//  ***************************************************************************
/// @brief  Write data to RAM from external buffer 
/// @param  ram_address: RAM address
/// @param  buffer: pointer to buffer
/// @param  bytes_count: bytes count for copy
/// @return true - read data success, false - error
//  ***************************************************************************
bool ram_map_write(uint32_t ram_address, const uint8_t* buffer, uint32_t bytes_count) {
	
	if (ram_address + bytes_count >= RAM_MAP_SIZE) {
		return false;
	}
    
    for (uint32_t i = 0; i < bytes_count; ++i) {

        if (ram_map.data[ram_address + i] != NULL) {
            *(ram_map.data[ram_address + i]) = buffer[i];
        }
    }
	
	return true;
}
