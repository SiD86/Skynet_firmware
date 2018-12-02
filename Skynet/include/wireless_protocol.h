//  ***************************************************************************
/// @file    wireless_protocol.h
/// @author  NeoProg
/// @brief   Wireless protocol
//  ***************************************************************************
#ifndef WIRELESS_PROTOCOL_H_
#define WIRELESS_PROTOCOL_H_


//
// Frame size - 40
//
typedef struct __attribute__ ((packed)) {
	
	uint32_t number;		// Frame number
	uint8_t data[32];		// Frame data
	uint32_t CRC;			// Frame CRC
	
} wireless_protocol_frame_t;


//
// Control data (PC -> CONTROLLER)
//
typedef struct __attribute__ ((packed)) {
	
	uint8_t command;
	uint8_t reserved[31];
	
} wireless_protocol_control_data_t;


//
// State data (CONTROLLER -> PC)
//
typedef struct __attribute__ ((packed)) {
	
	uint32_t error_status;
	
	uint16_t wireless_voltage;
	uint16_t periphery_voltage;
	uint16_t battery_bank0_voltage;
	uint16_t battery_bank1_voltage;
	uint16_t power_board_temperature;
	
	
	uint8_t reserved[18];
	
} wireless_protocol_state_data_t;


#endif /* WIRELESS_PROTOCOL_H_ */