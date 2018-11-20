//  ***************************************************************************
/// @file    error_handling.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include "error_handling.h"

#define FATAL_ERROR_MASK						(0x0001)
#define SELFDIAG_ERROR_MASK						(0x0002)
#define CONFIG_ERROR_MASK						(0x0004)
#define MEMORY_ERROR_MASK						(0x0008)
#define SYNC_ERROR_MASK							(0x0010)
#define OOR_ERROR_MASK							(0x0020)


uint32_t error_status = 0;


//  ***************************************************************************
/// @brief  Callback function for check error - Servo driver error
/// @param  none
/// @return none
//  ***************************************************************************
bool callback_is_servo_driver_error_set(void) {
	return error_status & ERROR_MODULE_SERVO_DRIVER;
}

//  ***************************************************************************
/// @brief  Callback function for check error - Limbs driver error
/// @param  none
/// @return none
//  ***************************************************************************
bool callback_is_limbs_driver_error_set(void) {
	return error_status & ERROR_MODULE_LIMBS_DRIVER;
}

//  ***************************************************************************
/// @brief  Callback function for check error - Gaits engine error
/// @param  none
/// @return none
//  ***************************************************************************
bool callback_is_gaits_engine_error_set(void) {
	return error_status & ERROR_MODULE_GAITS_ENGINE;
}



/// ***************************************************************************
/// @brief  Callback function for setting error - SelfDiag Error
/// @param  none
/// @return none
//  ***************************************************************************
void callback_set_selfdiag_error(error_module_name_t module) {
	error_status |= module | SELFDIAG_ERROR_MASK | FATAL_ERROR_MASK;
}

//  ***************************************************************************
/// @brief  Callback function for setting error - Configuration Error
/// @param  none
/// @return none
//  ***************************************************************************
void callback_set_config_error(error_module_name_t module) {
	error_status |= module | CONFIG_ERROR_MASK | FATAL_ERROR_MASK;
}

/// ***************************************************************************
/// @brief  Callback function for setting error - Memory Error
/// @param  none
/// @return none
//  ***************************************************************************
void callback_set_memory_error(error_module_name_t module) {
	error_status |= module | MEMORY_ERROR_MASK | FATAL_ERROR_MASK;
}

/// ***************************************************************************
/// @brief  Callback function for setting error - Sync Error
/// @param  none
/// @return none
//  ***************************************************************************
void callback_set_sync_error(error_module_name_t module) {
	error_status |= module | SYNC_ERROR_MASK | FATAL_ERROR_MASK;
}

/// ***************************************************************************
/// @brief  Callback function for setting error - Out Of Range Error
/// @param  none
/// @return none
//  ***************************************************************************
void callback_set_out_of_range_error(error_module_name_t module) {
	error_status |= module | OOR_ERROR_MASK;
}



/// ***************************************************************************
/// @brief  Callback function for check error - Fatal error
/// @param  none
/// @return none
//  ***************************************************************************
bool callback_is_fatal_error_set(void) {
	return error_status & FATAL_ERROR_MASK;
}