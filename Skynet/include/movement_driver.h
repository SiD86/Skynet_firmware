//  ***************************************************************************
/// @file    movement_driver.h
/// @author  NeoProg
/// @brief   Hexapod movement driver
//  ***************************************************************************
#ifndef MOVEMENT_DRIVER_H_
#define MOVEMENT_DRIVER_H_


typedef enum {
	SEQUENCE_NONE,
	SEQUENCE_BASE,
	SEQUENCE_UP,
	SEQUENCE_DOWN,
	SEQUENCE_DIRECT_MOVEMENT,
	SEQUENCE_REVERSE_MOVEMENT,
	SEQUENCE_ROTATE_LEFT,
	SEQUENCE_ROTATE_RIGHT,
    
    SUPPORT_SEQUENCE_COUNT
} sequence_t;


void movement_driver_init(void);
void movement_driver_process(void);
void movement_driver_select_sequence(sequence_t sequence);


#endif /* MOVEMENT_DRIVER_H_ */