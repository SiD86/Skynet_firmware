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

#define DISABLE_PULSE_WIDTH						(0)

#define MG996R_MIN_PULSE_WIDTH                  (880)
#define MG996R_MAX_PULSE_WIDTH                  (2280)
#define MG996R_MAX_SERVO_ANGLE                  (150)

#define DS3218_MIN_PULSE_WIDTH                  (500)
#define DS3218_MAX_PULSE_WIDTH                  (2500)
#define DS3218_MAX_SERVO_ANGLE                  (270)

#define OVERRIDE_DISABLE_VALUE                  (0x7F)


// Servo direction
typedef enum {
    DIRECTION_DIRECT,    // MAX -> MIN
    DIRECTION_REVERSE    // MIN -> MAX
} direction_t;

// Servo type
typedef enum {
    SERVO_TYPE_MG996R,
    SERVO_TYPE_DS3218
} servo_type_t;

// Servo information
typedef struct {
    float        angle;					// Current servo angle, [degree] 
    servo_type_t type;					// Servo type
    direction_t  direction;				// Servo rotate direction
	uint32_t     physic_zero_trim;      // Servo physic zero trim, [degree]
    uint32_t     start_logical_zero;    // Servo logic zero, [degree]
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
	
    // Calculate servo angle
    servo_channels[ch].angle = servo_channels[ch].start_logical_zero + angle;
	
	// Check angle
	if (servo_channels[ch].angle < 0) {
		callback_set_out_of_range_error(ERROR_MODULE_SERVO_DRIVER);
	}
    if (servo_channels[ch].type == SERVO_TYPE_MG996R && servo_channels[ch].angle > MG996R_MAX_SERVO_ANGLE) {
        callback_set_out_of_range_error(ERROR_MODULE_SERVO_DRIVER);
    }
	else if (servo_channels[ch].type == SERVO_TYPE_DS3218 && servo_channels[ch].angle > DS3218_MAX_SERVO_ANGLE) {
        callback_set_out_of_range_error(ERROR_MODULE_SERVO_DRIVER);
    }
    
    // Apply physic zero trim
    servo_channels[ch].angle += servo_channels[ch].physic_zero_trim;
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
					servo_channels[i].angle = servo_channels[i].start_logical_zero + ram_servo_angle_override[i] + servo_channels[i].physic_zero_trim;
				}
					
				// Calculate and load pulse width
				pwm_set_width(i, convert_angle_to_pulse_width(&servo_channels[i]));
				
				// Update RAM variable
				ram_servo_angle[i] = servo_channels[i].angle;
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
    
    for (uint32_t i = 0; i < SUPPORT_SERVO_COUNT; ++i) {
        
        uint32_t base_address = i * SERVO_CONFIGURATION_SIZE;
        
        // Read servo type
        uint8_t servo_type = veeprom_read_8(base_address + SERVO_TYPE_EE_ADDRESS);
        if (servo_type != SERVO_TYPE_MG996R && servo_type != SERVO_TYPE_DS3218) {
            return false;
        }
        
        // Read servo rotate direction
        uint8_t direction = veeprom_read_8(base_address + SERVO_ROTATE_DIRECTION_EE_ADDRESS);
        if (direction != DIRECTION_DIRECT && direction != DIRECTION_REVERSE) {
            return false;
        }
        
        // Read physic zero trim
        uint8_t physic_zero_trim = veeprom_read_8(base_address + SERVO_PHYSIC_ZERO_TRIM_EE_ADDRESS);
        if (servo_type == SERVO_TYPE_MG996R && physic_zero_trim > MG996R_MAX_SERVO_ANGLE) {
            return false;
        }
        if (servo_type == SERVO_TYPE_DS3218 && physic_zero_trim > DS3218_MAX_SERVO_ANGLE) {
            return false;
        }
        
        // Read start logical zero
        int32_t start_logical_zero = veeprom_read_8(base_address + SERVO_START_LOGICAL_ZERO_EE_ADDRESS);
        if (servo_type == SERVO_TYPE_MG996R && start_logical_zero > MG996R_MAX_SERVO_ANGLE) {
            return false;
        }
        if (servo_type == SERVO_TYPE_DS3218 && start_logical_zero > DS3218_MAX_SERVO_ANGLE) {
            return false;
        }
        
        // Fill information
        servo_channels[i].angle = physic_zero_trim + start_logical_zero;
        servo_channels[i].type = servo_type;
        servo_channels[i].direction = direction;
		servo_channels[i].physic_zero_trim = physic_zero_trim;
        servo_channels[i].start_logical_zero = start_logical_zero;
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
    
    if (servo_info->type == SERVO_TYPE_MG996R) {
    
        uint32_t pulse_width = servo_info->angle * (MG996R_MAX_PULSE_WIDTH - MG996R_MIN_PULSE_WIDTH) / MG996R_MAX_SERVO_ANGLE + MG996R_MIN_PULSE_WIDTH;
        if (servo_info->direction == DIRECTION_REVERSE) {
            pulse_width = MG996R_MAX_PULSE_WIDTH - (pulse_width - MG996R_MIN_PULSE_WIDTH);
        }
        return pulse_width;
    }  
    else if (servo_info->type == SERVO_TYPE_DS3218) {
        
        uint32_t pulse_width = servo_info->angle * (DS3218_MAX_PULSE_WIDTH - DS3218_MIN_PULSE_WIDTH) / DS3218_MAX_SERVO_ANGLE + DS3218_MIN_PULSE_WIDTH;
        if (servo_info->direction == DIRECTION_REVERSE) {
            pulse_width = DS3218_MAX_PULSE_WIDTH - (pulse_width - DS3218_MIN_PULSE_WIDTH);
        }
        return pulse_width;
    }
    
    callback_set_internal_error(ERROR_MODULE_SERVO_DRIVER);
    return DISABLE_PULSE_WIDTH;
}