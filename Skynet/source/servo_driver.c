//  ***************************************************************************
/// @file    servo_driver.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "servo_driver.h"
#include "pwm.h"
#include "veeprom.h"
#include "veeprom_map.h"
#include "error_handling.h"

#define MG996R_DISABLE_PULSE_WIDTH              (0)
#define MG996R_MIN_PULSE_WIDTH                  (900)
#define MG996R_MAX_PULSE_WIDTH                  (2300)
#define MG996R_MAX_SERVO_ANGLE                  (155)

#define OVERRIDE_SMOOTH_ITERATION_COUNT         (50)
#define OVERRIDE_DISABLE_VALUE                  (0x7F)


// Servo direction
typedef enum {
    DIRECTION_DIRECT,    // MAX -> MIN
    DIRECTION_REVERSE    // MIN -> MAX
} servo_direction_t;

// Servo information
typedef struct {

    float current_angle;                // Current servo angle, [degree]
    float dest_angle;                   // Destination servo angle, [degree]
    
    // Internal configuration
    uint32_t          zero_offset;      // Servo zero offset, [degree]
    servo_direction_t direction;        // Move direction
    
    // For smooth algorithm usage
    float    smooth_delta;              // Offset servo angle for one iteration, [degree]
    uint32_t smooth_iter_count;         // Movement iteration count

} servo_info_t;

// Servo driver states
typedef enum {
    STATE_NOINIT,
    STATE_STARTUP,
    STATE_PROCESS,
    STATE_WAIT_NEXT_PWM_PERIOD
} servo_driver_state_t;


int8_t  ram_servo_dest_angle_override[SUPPORT_SERVO_COUNT] = {0};   // Read only
uint8_t ram_servo_current_angle[SUPPORT_SERVO_COUNT] = {0};         // Write only
    
static servo_driver_state_t driver_state = STATE_NOINIT;
static servo_info_t         servo_channels[SUPPORT_SERVO_COUNT] = { 0 };


static bool read_configuration(void);
static void smooth_algorithm_process(servo_info_t* servo_info);
static uint32_t convert_angle_to_pulse_width(const servo_info_t* servo_info);


//  ***************************************************************************
/// @brief  Servo driver initialize
/// @param  none
/// @return none
//  ***************************************************************************
void servo_driver_init(void) {
    
    if (read_configuration() == false) {
        callback_set_config_error(ERROR_MODULE_SERVO_DRIVER);
        return;
    }
    
    // Disable override
    for (uint32_t i = 0; i < SUPPORT_SERVO_COUNT; ++i) {
        ram_servo_dest_angle_override[i] = OVERRIDE_DISABLE_VALUE;
    }

    pwm_init();
    pwm_start();
    
    driver_state = STATE_STARTUP;
}

//  ***************************************************************************
/// @brief  Start smooth move servo to new position
/// @param  servo: servo index
/// @param  angle: destination angle
/// @param  iteration_count: iteration count for movement complete
/// @return none
//  ***************************************************************************
void servo_driver_start_move(uint32_t ch, int32_t angle, uint32_t iteration_count) {
    
    if (ch >= SUPPORT_SERVO_COUNT || iteration_count == 0) {
        callback_set_selfdiag_error(ERROR_MODULE_SERVO_DRIVER);
        return;
    }
    
    
    servo_channels[ch].dest_angle   = servo_channels[ch].zero_offset + angle;
    servo_channels[ch].smooth_delta = (servo_channels[ch].dest_angle - servo_channels[ch].current_angle) / iteration_count;
    if (servo_channels[ch].smooth_delta != 0) {
        servo_channels[ch].smooth_iter_count = iteration_count;
    }
    else {
        servo_channels[ch].smooth_iter_count = 0;
    }
    
    if (servo_channels[ch].dest_angle > MG996R_MAX_SERVO_ANGLE) {
        callback_set_out_of_range_error(ERROR_MODULE_SERVO_DRIVER);
        return;
    }
}

//  ***************************************************************************
/// @brief  Stop move servo
/// @param  servo: servo index
/// @return none
//  ***************************************************************************
void servo_driver_stop_move(uint32_t ch) {

    if (ch >= SUPPORT_SERVO_COUNT) {
        callback_set_selfdiag_error(ERROR_MODULE_SERVO_DRIVER);
        return;
    }

    // Stop movement algorithm
    servo_channels[ch].dest_angle = servo_channels[ch].current_angle;
    servo_channels[ch].smooth_iter_count = 0;
    servo_channels[ch].smooth_delta = 0;
}

//  ***************************************************************************
/// @brief  Check all servo in move complete
/// @param  none
/// @return none
//  ***************************************************************************
bool servo_driver_is_move_complete(void) {
    
    for (uint32_t i = 0; i < SUPPORT_SERVO_COUNT; ++i) {
        
        if (servo_channels[i].smooth_iter_count != 0) {
            return false; // One or more servos movement not complete
        }
    }
    return true;
}

//  ***************************************************************************
/// @brief  Servo driver process
/// @param  none
/// @return none
//  ***************************************************************************
void servo_driver_process(void) {
    
    if (callback_is_servo_driver_error_set() == true) {
        pwm_stop();
        return; // Module disabled
    }
    

    static uint32_t prev_counter_value = 0;
    
    uint32_t counter_value = pwm_get_counter();
    switch (driver_state)
    {
    
    case STATE_STARTUP:
        prev_counter_value = counter_value;
        driver_state = STATE_WAIT_NEXT_PWM_PERIOD;
        break;

    case STATE_PROCESS:
    
        // Update position for all servo
        for (uint32_t i = 0; i < SUPPORT_SERVO_COUNT; ++i) {
            
            // Override process
            if (ram_servo_dest_angle_override[i] != OVERRIDE_DISABLE_VALUE) {
                
                if (servo_channels[i].dest_angle != ram_servo_dest_angle_override[i]) {
                    servo_driver_start_move(i, ram_servo_dest_angle_override[i], OVERRIDE_SMOOTH_ITERATION_COUNT);
                }
            }
            
            smooth_algorithm_process(&servo_channels[i]);
            pwm_set_width(i, convert_angle_to_pulse_width(&servo_channels[i]));
        }
        
        driver_state = STATE_WAIT_NEXT_PWM_PERIOD;
        break;
        
    case STATE_WAIT_NEXT_PWM_PERIOD:
    
        // Wait PWM next period
        if (prev_counter_value != counter_value) {
            
            if (counter_value - prev_counter_value > 1) { // We skip sync timer tick
                callback_set_sync_error(ERROR_MODULE_SERVO_DRIVER);
            }
            
            prev_counter_value = counter_value;
            driver_state = STATE_PROCESS;
        }
        else { // IDLE state
            
            // Update RAM variables
            for (uint32_t i = 0; i < SUPPORT_SERVO_COUNT; ++i) {
                ram_servo_current_angle[i] = servo_channels[i].current_angle;
            }
        }
        break;
        
    case STATE_NOINIT:
    default:
        callback_set_selfdiag_error(ERROR_MODULE_SERVO_DRIVER);
        break;
    }
}

//  ***************************************************************************
/// @brief  Read configuration
/// @param    none
/// @return true - read success, false - fail
//  ***************************************************************************
static bool read_configuration(void) {
    
    for (uint32_t i = 0; i < SUPPORT_SERVO_COUNT; ++i) {
        
        uint32_t base_address = i * SERVO_CONFIGURATION_SIZE;
        
        // Read servo zero offset
        uint8_t servo_zero_offset = veeprom_read_8(base_address + SERVO_ZERO_OFFSET_EE_ADDRESS);
        if (servo_zero_offset > MG996R_MAX_SERVO_ANGLE) {
            return false;
        }
        
        // Read servo rotate direction
        uint8_t direction = veeprom_read_8(base_address + SERVO_ROTATE_DIRECTION_EE_ADDRESS);
        if (direction != DIRECTION_DIRECT && direction != DIRECTION_REVERSE) {
            return false;
        }
        
        // Read start angle
        uint8_t start_angle = servo_zero_offset + veeprom_read_8(base_address + SERVO_START_ANGLE_EE_ADDRESS);
        if (start_angle > MG996R_MAX_SERVO_ANGLE) {
            return false;
        }
        
        // Fill information
        servo_channels[i].current_angle = start_angle;
        servo_channels[i].dest_angle    = start_angle;
        
        servo_channels[i].zero_offset   = servo_zero_offset;
        servo_channels[i].direction     = direction;
        
        servo_channels[i].smooth_delta = 0;
        servo_channels[i].smooth_iter_count = 0;
    }

    return true;
}

//  ***************************************************************************
/// @brief  Update current angle for servo
/// @param  servo_info: @ref servo_info_t
/// @return none
//  ***************************************************************************
static void smooth_algorithm_process(servo_info_t* servo_info) {
    
    // Calculate new servo angle
    if (servo_info->smooth_iter_count > 0) {
        servo_info->current_angle += servo_info->smooth_delta;
        --servo_info->smooth_iter_count;
    }
    else {
        // On last iteration current angle must be equal destination angle (for remove calculation error)
        servo_info->current_angle = servo_info->dest_angle; 
    }
}

//  ***************************************************************************
/// @brief  Convert servo angle to PWM pulse width
/// @note    This is Arduino map function
/// @param  angle: current servo angle
/// @return PWM pulse width
//  ***************************************************************************
static uint32_t convert_angle_to_pulse_width(const servo_info_t* servo_info) {
    
    uint32_t pulse_width = servo_info->current_angle * (MG996R_MAX_PULSE_WIDTH - MG996R_MIN_PULSE_WIDTH) / MG996R_MAX_SERVO_ANGLE + MG996R_MIN_PULSE_WIDTH;
    if (servo_info->direction == DIRECTION_REVERSE) {
        pulse_width = MG996R_MAX_PULSE_WIDTH - (pulse_width - MG996R_MIN_PULSE_WIDTH);
    }
    
    return pulse_width;
}