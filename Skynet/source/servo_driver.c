//  ***************************************************************************
/// @file    servo_driver.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <stdbool.h>
#include "servo_driver.h"
#include "pwm.h"
#include "veeprom.h"
#include "veeprom_map.h"
#include "error_handling.h"

#define SERVO_DIRECTION_CW						(0x00)
#define SERVO_DIRECTION_CCW						(0x01)
#define SERVO_BIDIRECTIONAL_MODE_DISABLE		(0x00)
#define SERVO_BIDIRECTIONAL_MODE_ENABLE			(0x02)

#define DISABLE_PULSE_WIDTH						(0)
#define CALIBRATION_TABLE_MAX_SIZE              (27)
#define CALIBRATION_TABLE_STEP_SIZE             (10)

#define OVERRIDE_DISABLE_VALUE                  (0x7F)

// Servo information
typedef struct {
    float    physic_angle;			// Current servo physic angle, [degree] 
    uint32_t config;			    // Servo configuration
	uint32_t angle_currection;    	// Servo angle correction, [degree]
    uint32_t min_physic_angle;      // Servo min physic angle, [degree]
    uint32_t max_physic_angle;      // Servo max physic angle, [degree]
    uint16_t calibration_table[27]; // Calibration table
} servo_info_t;

// Servo driver states
typedef enum {
    STATE_NOINIT,
    STATE_STARTUP,
	STATE_WAIT_NEXT_PWM_PERIOD,
    STATE_LOAD_PULSE_WIDTH
} driver_state_t;


int8_t ram_servo_angle_override[SUPPORT_SERVO_COUNT] = {0};	// Write only
int8_t ram_servo_angle[SUPPORT_SERVO_COUNT] = {0};			// Read only
    
static driver_state_t driver_state = STATE_NOINIT;
static servo_info_t   servo_channels[SUPPORT_SERVO_COUNT] = { 0 };


static bool read_configuration(void);
static uint32_t convert_angle_to_pulse_width(const servo_info_t* servo_info);


//  ***************************************************************************
/// @brief  Servo driver initialization
/// @return none
//  ***************************************************************************
void servo_driver_init(void) {
    
    if (read_configuration() == false) {
        callback_set_config_error(ERROR_MODULE_SERVO_DRIVER);
        return;
    }
	
	// Initialization servo channels
    for (uint32_t i = 0; i < SUPPORT_SERVO_COUNT; ++i) {
        ram_servo_angle_override[i] = OVERRIDE_DISABLE_VALUE;
    }

    pwm_init();
    pwm_enable();
    
    driver_state = STATE_STARTUP;
}

//  ***************************************************************************
/// @brief  Start move servo to new angle
/// @param  ch:    servo channel
/// @param  angle: new angle
/// @return none
//  ***************************************************************************
void servo_driver_move(uint32_t ch, float angle) {
    
    if (ch >= SUPPORT_SERVO_COUNT) {
        callback_set_internal_error(ERROR_MODULE_SERVO_DRIVER);
        return;
    }
	
	servo_info_t* servo_info = &servo_channels[ch];
	
    // Calculate servo logical zero
	uint32_t logical_zero = 0;
	if ((servo_info->config & SERVO_CONFIG_BIDIRECTIONAL_MODE_MASK) == SERVO_BIDIRECTIONAL_MODE_ENABLE) {
		logical_zero = servo_info->max_physic_angle / 2;
	}
	
	// Calculate physic servo angle
    servo_info->physic_angle = logical_zero + angle;
	
	// Constrain physic servo angle
	if (servo_info->physic_angle < servo_info->min_physic_angle) {
		servo_info->physic_angle = servo_info->min_physic_angle;
	}
	
	// Apply angle correction
	servo_info->physic_angle += servo_info->angle_currection;
	
	// Constrain physic servo angle
	if (servo_info->physic_angle > servo_info->max_physic_angle) {
        servo_info->physic_angle = servo_info->max_physic_angle;
    }
}

//  ***************************************************************************
/// @brief  Servo driver process
/// @note   Call from main loop
//  ***************************************************************************
void servo_driver_process(void) {
    
    if (callback_is_servo_driver_error_set() == true) {
        pwm_disable();
        return; // Module disabled
    }
	

    static uint32_t prev_counter_value = 0;
    uint32_t counter_value = pwm_get_counter();
    
    switch (driver_state) {
        
        case STATE_STARTUP:
            prev_counter_value = counter_value;
            driver_state = STATE_WAIT_NEXT_PWM_PERIOD;
            break;
			
		case STATE_WAIT_NEXT_PWM_PERIOD:
			if (prev_counter_value != counter_value) {
			
				if (counter_value - prev_counter_value > 1) {
					// We skipped PWM period. Very long time between servo_driver_process() function calls
					callback_set_sync_error(ERROR_MODULE_SERVO_DRIVER);
				}
			
				prev_counter_value = counter_value;
				driver_state = STATE_LOAD_PULSE_WIDTH;
			}
			break;

        case STATE_LOAD_PULSE_WIDTH:
			pwm_set_buffers_state(PWM_BUFFERS_LOCK);
            for (uint32_t i = 0; i < SUPPORT_SERVO_COUNT; ++i) {
				
				// Override servo angle process
				if (ram_servo_angle_override[i] != OVERRIDE_DISABLE_VALUE) {
					servo_driver_move(i, ram_servo_angle_override[i]);
				}
					
				// Calculate and load pulse width
				pwm_set_width(i, convert_angle_to_pulse_width(&servo_channels[i]));
				
				// Update RAM variable
				ram_servo_angle[i] = servo_channels[i].physic_angle;
            }
            pwm_set_buffers_state(PWM_BUFFERS_UNLOCK);
			
            driver_state = STATE_WAIT_NEXT_PWM_PERIOD;
            break;
            
        case STATE_NOINIT:
        default:
            callback_set_internal_error(ERROR_MODULE_SERVO_DRIVER);
            break;
    }
}





//  ***************************************************************************
/// @brief  Read configuration
/// @param  none
/// @return true - read success, false - fail
//  ***************************************************************************
static bool read_configuration(void) {
    
    for (uint32_t servo_index = 0; servo_index < SUPPORT_SERVO_COUNT; ++servo_index) {
        
        uint32_t base_address = servo_index * SERVO_CONFIGURATION_SIZE;
        
        // Read servo configuration
        uint8_t config = veeprom_read_8(base_address + SERVO_CONFIG_EE_ADDRESS);
        if (config == 0xFF) {
            return false;
        }
        
        // Read angle correction
        uint32_t angle_correction = veeprom_read_8(base_address + SERVO_ANGLE_CORRECTION_EE_ADDRESS);

		// Read max physic angle
        uint32_t max_physic_angle = veeprom_read_16(base_address + SERVO_MAX_PHYSIC_ANGLE_EE_ADDRESS);
        if (max_physic_angle == 0xFFFF || angle_correction > max_physic_angle) {
            return false;
        }
		
        // Fill main information
        servo_channels[servo_index].physic_angle = 0;
        servo_channels[servo_index].config = config;
		servo_channels[servo_index].angle_currection = angle_correction;
        servo_channels[servo_index].max_physic_angle = max_physic_angle;

		// Read calibration table
		uint32_t max_table_point = max_physic_angle / CALIBRATION_TABLE_STEP_SIZE;
		for (uint32_t i = 0; i <= max_table_point; ++i) {
			servo_channels[servo_index].calibration_table[i] = veeprom_read_16(base_address + SERVO_CALIBRATION_TABLE_EE_ADDRESS + i * 2);
		}
		
		// Move servo to start position
		servo_driver_move(servo_index, 0);
    }

    return true;
}

//  ***************************************************************************
/// @brief  Convert servo angle to PWM pulse width
/// @note   This is Arduino map function
/// @param  servo_info: servo info @ref servo_info_t
/// @return PWM pulse width
//  ***************************************************************************
static uint32_t convert_angle_to_pulse_width(const servo_info_t* servo_info) {
    
    float angle = servo_info->physic_angle;
    if ((servo_info->config & SERVO_CONFIG_ROTATE_DIRECTION_MASK) == SERVO_DIRECTION_CCW) {
        angle = servo_info->max_physic_angle - angle;
    }
    
    uint32_t table_index = angle / CALIBRATION_TABLE_STEP_SIZE;

    if (angle < servo_info->max_physic_angle) {
        
        float first_value = servo_info->calibration_table[table_index];
        float second_value = servo_info->calibration_table[table_index + 1];
        
        float step = (second_value - first_value) / CALIBRATION_TABLE_STEP_SIZE;
        return first_value + step * ((uint32_t)angle % 10);
    }
	
	return servo_info->calibration_table[table_index];
}