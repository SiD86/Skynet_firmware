//  ***************************************************************************
/// @file    src.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include "scr.h"
#include "movement_driver.h"
#include "limbs_driver.h"
#include "veeprom.h"
#include "multimedia.h"

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
		
		case 0xFF:
			multimedia_update_image();
			break;
			
		/*case SCR_CMD_CALCULATE_CHECKSUM:
			veeprom_update_checksum();
			break;
			
		case SCR_CMD_SELECT_GAIT_UP:
			movement_driver_select_sequence(SEQUENCE_UP);
			break;
		
		case SCR_CMD_SELECT_GAIT_BASE:
			movement_driver_select_sequence(SEQUENCE_BASE);
			break;	
	
		case SCR_CMD_SELECT_GAIT_DOWN:
			movement_driver_select_sequence(SEQUENCE_DOWN);
			break;
			
		case SCR_CMD_SELECT_GAIT_DIRECT_MOVEMENT:
			movement_driver_select_sequence(SEQUENCE_DIRECT_MOVEMENT);
			break;
			
		case SCR_CMD_SELECT_GAIT_REVERSE_MOVEMENT:
			movement_driver_select_sequence(SEQUENCE_REVERSE_MOVEMENT);
			break;
		
		case SCR_CMD_SELECT_GAIT_ROTATE_LEFT:
			movement_driver_select_sequence(SEQUENCE_ROTATE_LEFT);
			break;
			
		case SCR_CMD_SELECT_GAIT_ROTATE_RIGHT:
			movement_driver_select_sequence(SEQUENCE_ROTATE_RIGHT);
			break;*/
			
		case SCR_CMD_RESET:
			REG_RSTC_CR = 0xA5000005;
			break;
	}
	
	scr = 0x00;
}