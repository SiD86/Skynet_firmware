//  ***************************************************************************
/// @file    gaits_engine.h
/// @author  NeoProg
/// @brief   Hexapod gaits engine
//  ***************************************************************************
#ifndef GAITS_ENGINE_H_
#define GAITS_ENGINE_H_


typedef enum {
	
	SEQUENCE_NONE,
	
	SEQUENCE_BASE,
	SEQUENCE_UP,
	SEQUENCE_DOWN,
	
	SEQUENCE_DIRECT_MOVEMENT,
	SEQUENCE_REVERSE_MOVEMENT,

	SEQUENCE_ROTATE_LEFT,
	SEQUENCE_ROTATE_RIGHT
	
} sequence_t;


void gaits_engine_init(void);
void gaits_engine_process(void);
void gaits_engine_select_sequence(sequence_t sequence);


#endif /* GAITS_ENGINE_H_ */