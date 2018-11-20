/*
 * gait_sequences_base.h
 *
 * Created: 14.11.2018 11:41:28
 *  Author: NeoProg
 */ 


#ifndef GAIT_SEQUENCES_BASE_H_
#define GAIT_SEQUENCES_BASE_H_


typedef struct {
	
	int32_t coxa[SUPPORT_LIMBS_COUNT];
	int32_t femur[SUPPORT_LIMBS_COUNT];
	int32_t tibia[SUPPORT_LIMBS_COUNT];
	
} sequence_step_t;

typedef struct {
	
	uint32_t movement_iteration_count;	// Iteration count for limb movement complete
	
	uint32_t steps_count;				// Sequence steps count
	uint32_t delay_between_steps;		// Delay between sequence steps, [ms]
	uint32_t begin_loop_step;			// Sequence step index witch begins loop
	sequence_step_t steps_list[20];		// Sequence steps list
	
} sequence_info_t;


#endif /* GAIT_SEQUENCES_BASE_H_ */