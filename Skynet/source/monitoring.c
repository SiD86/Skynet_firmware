//  ***************************************************************************
/// @file    monitoring.c
/// @author  NeoProg
/// @brief   Monitoring subsystem
//  ***************************************************************************
#include "monitoring.h"

#include <sam.h>
#include <stdbool.h>
#include "adc.h"
#include "dac.h"
#include "veeprom_map.h"
#include "veeprom.h"
#include "systimer.h"
#include "error_handling.h"

#define LOW_BATTERY_VOLTAGE_BEEP_PERIOD			(10000)	// ms
#define LOW_BATTERY_VOLTAGE_BEEP_ENABLE_TIME	(100)	// ms
#define LOW_BATTERY_VOLTAGE_BEEP_DISABLE_TIME	(200)	// ms

#define SUPPORT_ADC_CHANNEL_COUNT				(3)
#define WIRELESS_VOLTAGE_ADC_CH					(2)
#define PERIPHERY_VOLTAGE_ADC_CH				(1)
#define BATTERY_VOLTAGE_ADC_CH					(0)


typedef enum {
	STATE_NOINIT,
	STATE_WAIT_CONVERSION,
	STATE_CALCULATE
} state_t;

typedef enum {
	BEEP_STATE_IDLE,
	BEEP_STATE_CHECK,
	BEEP_STATE_WAIT,
	BEEP_STATE_ENABLE,
	BEEP_STATE_ENABLE_DELAY,
	BEEP_STATE_DISABLE,
	BEEP_STATE_DISABLE_DELAY
} beep_state_t;

typedef struct {
	uint32_t up_resist;
	uint32_t down_resist;	
} voltage_divisor_info_t;


static state_t subsystem_state = STATE_NOINIT;
static voltage_divisor_info_t voltage_divisor[SUPPORT_ADC_CHANNEL_COUNT] = {0};
static uint32_t battery_low_voltage_threshold = 0;

uint16_t wireless_voltage = 0;	// 0.1 V
uint16_t periphery_voltage = 0;	// 0.1 V
uint16_t battery_voltage = 0;	// 0.1 V


static bool read_configuration(void);
static uint16_t calculate_voltage(float adc_voltage, const voltage_divisor_info_t* divisor_info);


//  ***************************************************************************
/// @brief	Monitoring subsystem initialization
/// @return	none
//  ***************************************************************************
void monitoring_init(void) {
	
	if (read_configuration() == false) {
		callback_set_config_error(ERROR_MODULE_MONITORING);
		return;
	}
	
	adc_init();
	adc_start_conversion();
	
	dac_init();
	
	subsystem_state = STATE_WAIT_CONVERSION;
}

//  ***************************************************************************
/// @brief	Monitoring subsystem process
/// @note	Call from main loop
//  ***************************************************************************
void monitoring_process(void) {
	
	if (callback_is_monitoring_error_set() == true) {
		dac_set_output_value(0, 0);
		return; // Module disabled
	}
	
	static beep_state_t beep_state = BEEP_STATE_IDLE;
	static float adc_voltage[SUPPORT_ADC_CHANNEL_COUNT] = {0};
	switch (subsystem_state) {
		
		case STATE_WAIT_CONVERSION:
			if (adc_is_conversion_complete() == true) {
				
				adc_voltage[0] = adc_get_voltage(WIRELESS_VOLTAGE_ADC_CH);
				adc_voltage[1] = adc_get_voltage(PERIPHERY_VOLTAGE_ADC_CH);
				adc_voltage[2] = adc_get_voltage(BATTERY_VOLTAGE_ADC_CH);
				
				adc_start_conversion();
				subsystem_state = STATE_CALCULATE;
			}
			break;
		
		case STATE_CALCULATE:
			wireless_voltage  = calculate_voltage(adc_voltage[0], &voltage_divisor[0]);
			periphery_voltage = calculate_voltage(adc_voltage[1], &voltage_divisor[1]);
			battery_voltage   = calculate_voltage(adc_voltage[2], &voltage_divisor[2]);
			
			beep_state = BEEP_STATE_CHECK;
			subsystem_state = STATE_WAIT_CONVERSION;
			break;
		
		case STATE_NOINIT:
		default:
			callback_set_internal_error(ERROR_MODULE_MONITORING);
			break;
	}
	
	
	
	static uint32_t prev_time = 0;
	static uint32_t beep_count = 0;
	switch (beep_state) {
		
		case BEEP_STATE_IDLE:
			break;
		
		case BEEP_STATE_CHECK:
			if (battery_voltage < battery_low_voltage_threshold) {
				beep_state = BEEP_STATE_WAIT;
			}
			break;
			
		case BEEP_STATE_WAIT:
			if (get_time_ms() - prev_time > LOW_BATTERY_VOLTAGE_BEEP_PERIOD) {
				prev_time = get_time_ms();
				beep_state = BEEP_STATE_ENABLE;
			}
			break;
		
		case BEEP_STATE_ENABLE:
			prev_time = get_time_ms();
			dac_set_output_value(0, 4095);
			beep_state = BEEP_STATE_ENABLE_DELAY;
			break;
			
		case BEEP_STATE_ENABLE_DELAY:
			if (get_time_ms() - prev_time > LOW_BATTERY_VOLTAGE_BEEP_ENABLE_TIME) {
				beep_state = BEEP_STATE_DISABLE;
			}
			break;
		
		case BEEP_STATE_DISABLE:
			prev_time = get_time_ms();
			dac_set_output_value(0, 0);
			beep_state = BEEP_STATE_DISABLE_DELAY;
			break;
			
		case BEEP_STATE_DISABLE_DELAY:
			if (get_time_ms() - prev_time > LOW_BATTERY_VOLTAGE_BEEP_DISABLE_TIME) {
				
				if (++beep_count >= 3) {
					beep_count = 0;
					beep_state = BEEP_STATE_CHECK;
				}
				else {
					beep_state = BEEP_STATE_ENABLE;
				}
			}
			break;
		
		default:
			callback_set_internal_error(ERROR_MODULE_MONITORING);
			break;
	}
}





//  ***************************************************************************
/// @brief  Read configuration
/// @param  none
/// @return true - read success, false - fail
//  ***************************************************************************
static bool read_configuration(void) {
	
	for (uint32_t i = 0; i < SUPPORT_ADC_CHANNEL_COUNT; ++i) {
		
		uint32_t base_address = VOLTAGE_DIVISOR_CONFIGURATION_BASE_EE_ADDRESS + i * VOLTAGE_DIVISOR_CONFIGURATION_SIZE;
		
		voltage_divisor[i].up_resist   = veeprom_read_32(base_address + VOLTAGE_DIVISOR_UP_RESIST_OFFSET);
		voltage_divisor[i].down_resist = veeprom_read_32(base_address + VOLTAGE_DIVISOR_DOWN_RESIST_OFFSET);
		if (voltage_divisor[i].up_resist == 0xFFFFFFFF || voltage_divisor[i].down_resist == 0xFFFFFFFF) {
			return false;
		}
	}
	
	battery_low_voltage_threshold = veeprom_read_16(BATTERY_LOW_VOLTAGE_THRESHOLD_EE_ADDRESS);
	if (battery_low_voltage_threshold == 0xFFFF) {
		return false;
	}
	
	return true;
}

//  ***************************************************************************
/// @brief	Calculate resistance divisor supply voltage
/// @param	adc_voltage: ADC voltage, [V]
/// @param	up_resistor: up resistor value, [Ohm]
/// @param	down_resistor: down resistor value, [Ohm]
/// @return	Resistance divisor supply voltage, [0.1 V]
//  ***************************************************************************
static uint16_t calculate_voltage(float adc_voltage, const voltage_divisor_info_t* divisor_info) {
	
	float divisor_factor = ((float)divisor_info->up_resist + (float)divisor_info->down_resist) / (float)divisor_info->down_resist;
	
	return (adc_voltage * divisor_factor) * 10.0f;
}