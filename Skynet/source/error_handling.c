//  ***************************************************************************
/// @file    error_handling.c
/// @author  NeoProg
//  ***************************************************************************
#include "error_handling.h"
#include <sam.h>

#define EMERGENCY_MODE_MASK				(0x0001)
#define INTERNAL_ERROR_MASK				(0x0002)
#define CONFIG_ERROR_MASK				(0x0004)
#define MEMORY_ERROR_MASK				(0x0008)
#define SYNC_ERROR_MASK					(0x0010)
#define OOR_ERROR_MASK					(0x0020)
#define I2C_ERROR_MASK					(0x0040)


uint32_t error_status = 0;


//  ***************************************************************************
/// @brief  Callback function for check error - Servo driver error
/// @return true - error bit set, false - no
//  ***************************************************************************
bool callback_is_servo_driver_error_set(void) {
	return error_status & ERROR_MODULE_SERVO_DRIVER;
}

//  ***************************************************************************
/// @brief  Callback function for check error - Limbs driver error
/// @return true - error bit set, false - no
//  ***************************************************************************
bool callback_is_limbs_driver_error_set(void) {
	return error_status & ERROR_MODULE_LIMBS_DRIVER;
}

//  ***************************************************************************
/// @brief  Callback function for check error - Gaits engine error
/// @return true - error bit set, false - no
//  ***************************************************************************
bool callback_is_gaits_engine_error_set(void) {
	return error_status & ERROR_MODULE_GAITS_ENGINE;
}

//  ***************************************************************************
/// @brief  Callback function for check error - Wireless modbus error
/// @return true - error bit set, false - no
//  ***************************************************************************
bool callback_is_wireless_modbus_error_set(void) {
	return error_status & ERROR_MODULE_WIRELESS_MODBUS;
}

//  ***************************************************************************
/// @brief  Callback function for check error - Monitoring error
/// @return true - error bit set, false - no
//  ***************************************************************************
bool callback_is_monitoring_error_set(void) {
	return error_status & ERROR_MODULE_MONITORING;
}

//  ***************************************************************************
/// @brief  Callback function for check error - OLED GL error
/// @return true - error bit set, false - no
//  ***************************************************************************
bool callback_is_oled_gl_error_set(void) {
    return error_status & ERROR_MODULE_OLED_GL;
}



/// ***************************************************************************
/// @brief  Callback function for set error - SelfDiag Error
/// @param  @ref error_module_name_t
//  ***************************************************************************
void callback_set_internal_error(error_module_name_t module) {
	error_status |= (module | INTERNAL_ERROR_MASK | EMERGENCY_MODE_MASK);
}

//  ***************************************************************************
/// @brief  Callback function for set error - Configuration Error
/// @param  @ref error_module_name_t
//  ***************************************************************************
void callback_set_config_error(error_module_name_t module) {
	error_status |= (module | CONFIG_ERROR_MASK | EMERGENCY_MODE_MASK);
}

/// ***************************************************************************
/// @brief  Callback function for set error - Memory Error
/// @param  @ref error_module_name_t
//  ***************************************************************************
void callback_set_memory_error(error_module_name_t module) {
	error_status |= (module | MEMORY_ERROR_MASK | EMERGENCY_MODE_MASK);
}

/// ***************************************************************************
/// @brief  Callback function for set error - Sync Error
/// @param  @ref error_module_name_t
//  ***************************************************************************
void callback_set_sync_error(error_module_name_t module) {
	error_status |= (module | SYNC_ERROR_MASK);
}

/// ***************************************************************************
/// @brief  Callback function for set error - Out Of Range Error
/// @param  @ref error_module_name_t
//  ***************************************************************************
void callback_set_out_of_range_error(error_module_name_t module) {
	error_status |= (module | OOR_ERROR_MASK);
}

/// ***************************************************************************
/// @brief  Callback function for set error - I2C Error
/// @param  @ref error_module_name_t
//  ***************************************************************************
void callback_set_i2c_error(error_module_name_t module) {
    error_status |= (module | I2C_ERROR_MASK);
}



/// ***************************************************************************
/// @brief  Callback function for check - Emergency Mode
/// @return true - emergency mode is active, false - no
//  ***************************************************************************
bool callback_is_emergency_mode_active(void) {
	return error_status & EMERGENCY_MODE_MASK;
}

/// ***************************************************************************
/// @brief  Callback function for check any error
/// @return true - status contain any error, false - no
//  ***************************************************************************
bool callback_is_any_error_set(void) {
    return error_status != 0;
}