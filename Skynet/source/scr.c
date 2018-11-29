//  ***************************************************************************
/// @file    veeprom.h
/// @author  NeoProg
/// @brief   Virtual EEPROM driver
//  ***************************************************************************
#include <sam.h>
#include "scr.h"
#include "gaits_engine.h"
#include "veeprom.h"

#define SCR_CMD_CALCULATE_CHECKSUM				(0x01)

#define SCR_CMD_SELECT_GAIT_UP					(0x02)
#define SCR_CMD_SELECT_GAIT_BASE				(0x03)
#define SCR_CMD_SELECT_GAIT_DOWN				(0x04)
#define SCR_CMD_SELECT_GAIT_DIRECT_MOVEMENT		(0x05)
#define SCR_CMD_SELECT_GAIT_REVERSE_MOVEMENT	(0x06)
#define SCR_CMD_SELECT_GAIT_ROTATE_LEFT			(0x07)
#define SCR_CMD_SELECT_GAIT_ROTATE_RIGHT		(0x08)

#define SCR_CMD_RESET							(0xAA)


uint8_t scr = 0;


//  ***************************************************************************
/// @brief	Special command register process
/// @return	none
//  ***************************************************************************
void scr_process(void) {
	
	switch (scr) {
		
		case SCR_CMD_CALCULATE_CHECKSUM:
			veeprom_update_checksum();
			break;
			
		case SCR_CMD_SELECT_GAIT_UP:
			gaits_engine_select_sequence(SEQUENCE_UP);
			break;
		
		case SCR_CMD_SELECT_GAIT_BASE:
			gaits_engine_select_sequence(SEQUENCE_BASE);
			break;	
	
		case SCR_CMD_SELECT_GAIT_DOWN:
			gaits_engine_select_sequence(SEQUENCE_DOWN);
			break;
			
		case SCR_CMD_SELECT_GAIT_DIRECT_MOVEMENT:
			gaits_engine_select_sequence(SEQUENCE_DIRECT_MOVEMENT);
			break;
			
		case SCR_CMD_SELECT_GAIT_REVERSE_MOVEMENT:
			gaits_engine_select_sequence(SEQUENCE_REVERSE_MOVEMENT);
			break;
		
		case SCR_CMD_SELECT_GAIT_ROTATE_LEFT:
			gaits_engine_select_sequence(SEQUENCE_ROTATE_LEFT);
			break;
			
		case SCR_CMD_SELECT_GAIT_ROTATE_RIGHT:
			gaits_engine_select_sequence(SEQUENCE_ROTATE_RIGHT);
			break;
			
		case SCR_CMD_RESET:
			REG_RSTC_CR = 0xA5000005;
			break;
	}
	
	scr = 0x00;
}