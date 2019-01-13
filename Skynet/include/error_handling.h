//  ***************************************************************************
/// @file    error_handling.h
/// @author  NeoProg
/// @brief   Error handling definitions
//  ***************************************************************************
#ifndef _ERROR_HANDLING_H_
#define _ERROR_HANDLING_H_

#include <stdbool.h>

typedef enum {
	ERROR_MODULE_NO				 = 0x00000000,
	ERROR_MODULE_VEEPROM		 = 0x00010000,
	ERROR_MODULE_SERVO_DRIVER    = 0x00020000,
	ERROR_MODULE_LIMBS_DRIVER    = 0x00040000,
	ERROR_MODULE_GAITS_ENGINE    = 0x00080000,
	ERROR_MODULE_WIRELESS_MODBUS = 0x00100000,
	ERROR_MODULE_MONITORING		 = 0x00200000,
    ERROR_MODULE_LED             = 0x00400000,
    ERROR_MODULE_MULTIMEDIA      = 0x00800000
} error_module_name_t;


extern uint32_t error_status;


extern bool callback_is_servo_driver_error_set(void);
extern bool callback_is_limbs_driver_error_set(void);
extern bool callback_is_gaits_engine_error_set(void);
extern bool callback_is_wireless_modbus_error_set(void);
extern bool callback_is_monitoring_error_set(void);
extern bool callback_is_multimedia_error_set(void);

extern void callback_set_internal_error(error_module_name_t module);
extern void callback_set_config_error(error_module_name_t module);
extern void callback_set_memory_error(error_module_name_t module);
extern void callback_set_sync_error(error_module_name_t module);
extern void callback_set_out_of_range_error(error_module_name_t module);
extern void callback_set_hardware_error(error_module_name_t module);

extern bool callback_is_emergency_mode_active(void);
extern bool callback_is_any_error_set(void);

#endif // _ERROR_HANDLING_H_