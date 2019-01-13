//  ***************************************************************************
/// @file    limbs_driver.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <fastmath.h>
#include "limbs_driver.h"
#include "servo_driver.h"
#include "veeprom.h"
#include "veeprom_map.h"
#include "systimer.h"
#include "error_handling.h"
#define RAD_TO_DEG(rad)                ((rad) * 180 / M_PI)
#define DEG_TO_RAD(deg)                ((deg) * M_PI / 180)

#define DEFAULT_ITERATION_COUNT        (50)
#define WAIT_TIME                      (10)


// Servo driver states
typedef enum {
    STATE_NOINIT,
    STATE_IDLE,
    STATE_CALC,
    STATE_LOAD,
    STATE_WAIT
} driver_state_t;

typedef enum {
    LINK_COXA,
    LINK_FEMUR,
    LINK_TIBIA
} link_id_t;

typedef enum {
    LIMB_FRONT_LEFT,
    LIMB_CENTER_LEFT,
    LIMB_REAR_LEFT,
    LIMB_FRONT_RIGHT,
    LIMB_CENTER_RIGHT,
    LIMB_REAR_RIGHT,
} limb_id_t;

typedef struct {
    
    // Current link state
    float angle;
    
    // Link configuration
    uint32_t length;
    int32_t  zero_rotate;
    int32_t  min_angle;
    int32_t  max_angle;
    
} link_info_t;

typedef struct {
    
    point_3d_t current_pos;
    point_3d_t dest_pos;
    point_3d_t delta_pos;
    uint32_t iterations_left;
    
    link_info_t links[3];
    point_3d_t begin_position;
    
} limb_info_t;


static driver_state_t driver_state = STATE_NOINIT;
static limb_info_t    limbs[SUPPORT_LIMB_COUNT] = {0};
static uint32_t       movement_iteration_count = DEFAULT_ITERATION_COUNT;


static bool read_configuration(void);
static bool kinematic_calculate_angles(limb_info_t* info);


//  ***************************************************************************
/// @brief  Limbs driver initialize
/// @param  none
/// @return none
//  ***************************************************************************
void limbs_driver_init(void) {
    
    if (read_configuration() == false) {
        callback_set_config_error(ERROR_MODULE_LIMBS_DRIVER);
        return;
    }
    
    // Calculate start link angles
    for (uint32_t i = 0; i < SUPPORT_LIMB_COUNT; ++i) {
        
        if (kinematic_calculate_angles(&limbs[i]) == false) {
            callback_set_config_error(ERROR_MODULE_LIMBS_DRIVER);
            return;
        }
    }
    
    // Initialization servo driver and set start servo angles
    servo_driver_init();
    for (uint32_t i = 0; i < SUPPORT_LIMB_COUNT; ++i) {
        servo_driver_set_angle(i * 3 + 0, limbs[i].links[LINK_COXA].angle);
        servo_driver_set_angle(i * 3 + 1, limbs[i].links[LINK_FEMUR].angle);
        servo_driver_set_angle(i * 3 + 2, limbs[i].links[LINK_TIBIA].angle);
    }
    servo_driver_move();
    
    // Initialization driver state
    movement_iteration_count = DEFAULT_ITERATION_COUNT;
    driver_state = STATE_IDLE;
}

//  ***************************************************************************
/// @brief  Start limb move
/// @param  point_list: point list
//  ***************************************************************************
void limbs_driver_start_move(point_3d_t* point_list) {
    
    if (movement_iteration_count == 0) {
        callback_set_internal_error(ERROR_MODULE_LIMBS_DRIVER);
        return;
    }
    
    // Prepare limbs for movement
    for (uint32_t i = 0; i < SUPPORT_LIMB_COUNT; ++i) {
        
        // Need start movement?
        if (point_list[i].x == LIMB_NO_MOVE || point_list[i].y == LIMB_NO_MOVE || point_list[i].z == LIMB_NO_MOVE) {
            continue;
        }
        if (limbs[i].current_pos.x == point_list[i].x && 
            limbs[i].current_pos.y == point_list[i].y && 
            limbs[i].current_pos.z == point_list[i].z) {
                
            continue;
        }
        
        // Prepare limb for movement
        limbs[i].dest_pos.x = point_list[i].x;
        limbs[i].dest_pos.y = point_list[i].y;
        limbs[i].dest_pos.z = point_list[i].z;
        
        limbs[i].delta_pos.x = (limbs[i].dest_pos.x - limbs[i].current_pos.x) / movement_iteration_count;
        limbs[i].delta_pos.y = (limbs[i].dest_pos.y - limbs[i].current_pos.y) / movement_iteration_count;
        limbs[i].delta_pos.z = (limbs[i].dest_pos.z - limbs[i].current_pos.z) / movement_iteration_count;
        limbs[i].iterations_left = movement_iteration_count;
        
        driver_state = STATE_CALC;
    }
}

//  ***************************************************************************
/// @brief  Stop limbs movement
/// @param  none
/// @return none
//  ***************************************************************************
void limbs_driver_stop_move(void) {
    
    driver_state = STATE_IDLE;
}

//  ***************************************************************************
/// @brief  Check all limbs movement complete
/// @param  none
/// @return true - move complete, false - movement in progress
//  ***************************************************************************
bool limbs_driver_is_move_complete(void) {
    
    return driver_state == STATE_IDLE;
}

//  ***************************************************************************
/// @brief  Set movement iteration count
/// @param  count: iteration count. Must be > 0
/// @return none
//  ***************************************************************************
void limbs_driver_set_movement_iteration_count(uint32_t count) {
    
    if (movement_iteration_count == 0) {
        callback_set_internal_error(ERROR_MODULE_LIMBS_DRIVER);
        return;
    }
    
    movement_iteration_count = count;
}

//  ***************************************************************************
/// @brief  Limbs driver process
/// @note   Call from main loop
//  ***************************************************************************
void limbs_driver_process(void) {
    
    if (callback_is_limbs_driver_error_set() == true) return;  // Module disabled


    static uint32_t begin_wait_time = 0;
    
    switch (driver_state) {
        
        case STATE_IDLE:
            break;
        
        case STATE_CALC:
            driver_state = STATE_IDLE;
            for (uint32_t i = 0; i < SUPPORT_LIMB_COUNT; ++i) {
                
                if (limbs[i].iterations_left != 0) { 
                
                    if (limbs[i].iterations_left > 1) {
                        
                        limbs[i].current_pos.x += limbs[i].delta_pos.x;
                        limbs[i].current_pos.y += limbs[i].delta_pos.y;
                        limbs[i].current_pos.z += limbs[i].delta_pos.z;
                        --limbs[i].iterations_left;
                    }
                    else if (limbs[i].iterations_left == 1) {
                        
                        // For avoid float point operations error
                        limbs[i].current_pos.x = limbs[i].dest_pos.x;
                        limbs[i].current_pos.y = limbs[i].dest_pos.y;
                        limbs[i].current_pos.z = limbs[i].dest_pos.z;
                        --limbs[i].iterations_left;
                    }
                    kinematic_calculate_angles(&limbs[i]);
                    driver_state = STATE_LOAD;
                }
            }
            break;
            
        case STATE_LOAD:
            for (uint32_t i = 0; i < SUPPORT_LIMB_COUNT; ++i) {
                servo_driver_set_angle(i * 3 + 0, limbs[i].links[LINK_COXA].angle);
                servo_driver_set_angle(i * 3 + 1, limbs[i].links[LINK_FEMUR].angle);
                servo_driver_set_angle(i * 3 + 2, limbs[i].links[LINK_TIBIA].angle);
            }
            servo_driver_move();
            
            begin_wait_time = get_time_ms();
            driver_state = STATE_WAIT;
            break;

        case STATE_WAIT:
            if (get_time_ms() - begin_wait_time >= WAIT_TIME) {
                driver_state = STATE_CALC;
            }
            break;
        
        case STATE_NOINIT:
        default:
            callback_set_internal_error(ERROR_MODULE_LIMBS_DRIVER);
            break;
    }
}



//  ***************************************************************************
/// @brief  Read configuration
/// @param  none
/// @return true - read success, false - fail
//  ***************************************************************************
static bool read_configuration(void) {
    
    //
    // Read kinematic configuration
    //
    limbs[LIMB_FRONT_LEFT].current_pos.x = 150;
    limbs[LIMB_FRONT_LEFT].current_pos.y = -50;
    limbs[LIMB_FRONT_LEFT].current_pos.z = 0;
   
    limbs[LIMB_FRONT_LEFT].links[LINK_COXA].length      = 45;
    limbs[LIMB_FRONT_LEFT].links[LINK_COXA].zero_rotate = 0/*45*/;
    limbs[LIMB_FRONT_LEFT].links[LINK_COXA].min_angle   = -90;
    limbs[LIMB_FRONT_LEFT].links[LINK_COXA].max_angle   = 90;
    
    limbs[LIMB_FRONT_LEFT].links[LINK_FEMUR].length      = 85;
    limbs[LIMB_FRONT_LEFT].links[LINK_FEMUR].zero_rotate = 125;
    limbs[LIMB_FRONT_LEFT].links[LINK_FEMUR].min_angle   = 0;
    limbs[LIMB_FRONT_LEFT].links[LINK_FEMUR].max_angle   = 120;
    
    limbs[LIMB_FRONT_LEFT].links[LINK_TIBIA].length      = 141;
    limbs[LIMB_FRONT_LEFT].links[LINK_TIBIA].zero_rotate = 40;
    limbs[LIMB_FRONT_LEFT].links[LINK_TIBIA].min_angle   = 0;
    limbs[LIMB_FRONT_LEFT].links[LINK_TIBIA].max_angle   = 120;
	
	
	limbs[LIMB_CENTER_LEFT].current_pos.x = 150;
    limbs[LIMB_CENTER_LEFT].current_pos.y = -50;
    limbs[LIMB_CENTER_LEFT].current_pos.z = 0;
   
    limbs[LIMB_CENTER_LEFT].links[LINK_COXA].length      = 45;
    limbs[LIMB_CENTER_LEFT].links[LINK_COXA].zero_rotate = 0;
    limbs[LIMB_CENTER_LEFT].links[LINK_COXA].min_angle   = -90;
    limbs[LIMB_CENTER_LEFT].links[LINK_COXA].max_angle   = 90;
    
    limbs[LIMB_CENTER_LEFT].links[LINK_FEMUR].length      = 85;
    limbs[LIMB_CENTER_LEFT].links[LINK_FEMUR].zero_rotate = 125;
    limbs[LIMB_CENTER_LEFT].links[LINK_FEMUR].min_angle   = 0;
    limbs[LIMB_CENTER_LEFT].links[LINK_FEMUR].max_angle   = 120;
    
    limbs[LIMB_CENTER_LEFT].links[LINK_TIBIA].length      = 141;
    limbs[LIMB_CENTER_LEFT].links[LINK_TIBIA].zero_rotate = 40;
    limbs[LIMB_CENTER_LEFT].links[LINK_TIBIA].min_angle   = 0;
    limbs[LIMB_CENTER_LEFT].links[LINK_TIBIA].max_angle   = 120;
	
	
	limbs[LIMB_REAR_LEFT].current_pos.x = 150;
    limbs[LIMB_REAR_LEFT].current_pos.y = -50;
    limbs[LIMB_REAR_LEFT].current_pos.z = 0;
   
    limbs[LIMB_REAR_LEFT].links[LINK_COXA].length      = 45;
    limbs[LIMB_REAR_LEFT].links[LINK_COXA].zero_rotate = 0/*-45*/;
    limbs[LIMB_REAR_LEFT].links[LINK_COXA].min_angle   = -90;
    limbs[LIMB_REAR_LEFT].links[LINK_COXA].max_angle   = 90;
    
    limbs[LIMB_REAR_LEFT].links[LINK_FEMUR].length      = 85;
    limbs[LIMB_REAR_LEFT].links[LINK_FEMUR].zero_rotate = 125;
    limbs[LIMB_REAR_LEFT].links[LINK_FEMUR].min_angle   = 0;
    limbs[LIMB_REAR_LEFT].links[LINK_FEMUR].max_angle   = 120;
    
    limbs[LIMB_REAR_LEFT].links[LINK_TIBIA].length      = 141;
    limbs[LIMB_REAR_LEFT].links[LINK_TIBIA].zero_rotate = 40;
    limbs[LIMB_REAR_LEFT].links[LINK_TIBIA].min_angle   = 0;
    limbs[LIMB_REAR_LEFT].links[LINK_TIBIA].max_angle   = 120;
    
    
    limbs[LIMB_FRONT_RIGHT].current_pos.x = 150;
    limbs[LIMB_FRONT_RIGHT].current_pos.y = -50;
    limbs[LIMB_FRONT_RIGHT].current_pos.z = 0;
   
    limbs[LIMB_FRONT_RIGHT].links[LINK_COXA].length      = 45;
    limbs[LIMB_FRONT_RIGHT].links[LINK_COXA].zero_rotate = 0/*45*/;
    limbs[LIMB_FRONT_RIGHT].links[LINK_COXA].min_angle   = -90;
    limbs[LIMB_FRONT_RIGHT].links[LINK_COXA].max_angle   = 90;
    
    limbs[LIMB_FRONT_RIGHT].links[LINK_FEMUR].length      = 85;
    limbs[LIMB_FRONT_RIGHT].links[LINK_FEMUR].zero_rotate = 125;
    limbs[LIMB_FRONT_RIGHT].links[LINK_FEMUR].min_angle   = 0;
    limbs[LIMB_FRONT_RIGHT].links[LINK_FEMUR].max_angle   = 120;
    
    limbs[LIMB_FRONT_RIGHT].links[LINK_TIBIA].length      = 141;
    limbs[LIMB_FRONT_RIGHT].links[LINK_TIBIA].zero_rotate = 40;
    limbs[LIMB_FRONT_RIGHT].links[LINK_TIBIA].min_angle   = 0;
    limbs[LIMB_FRONT_RIGHT].links[LINK_TIBIA].max_angle   = 120;
    
	
    limbs[LIMB_CENTER_RIGHT].current_pos.x = 150;
    limbs[LIMB_CENTER_RIGHT].current_pos.y = -50;
    limbs[LIMB_CENTER_RIGHT].current_pos.z = 0;

    limbs[LIMB_CENTER_RIGHT].links[LINK_COXA].length      = 45;
    limbs[LIMB_CENTER_RIGHT].links[LINK_COXA].zero_rotate = 0;
    limbs[LIMB_CENTER_RIGHT].links[LINK_COXA].min_angle   = -90;
    limbs[LIMB_CENTER_RIGHT].links[LINK_COXA].max_angle   = 90;

    limbs[LIMB_CENTER_RIGHT].links[LINK_FEMUR].length      = 85;
    limbs[LIMB_CENTER_RIGHT].links[LINK_FEMUR].zero_rotate = 125;
    limbs[LIMB_CENTER_RIGHT].links[LINK_FEMUR].min_angle   = 0;
    limbs[LIMB_CENTER_RIGHT].links[LINK_FEMUR].max_angle   = 120;

    limbs[LIMB_CENTER_RIGHT].links[LINK_TIBIA].length      = 141;
    limbs[LIMB_CENTER_RIGHT].links[LINK_TIBIA].zero_rotate = 40;
    limbs[LIMB_CENTER_RIGHT].links[LINK_TIBIA].min_angle   = 0;
    limbs[LIMB_CENTER_RIGHT].links[LINK_TIBIA].max_angle   = 120;
    
	
    limbs[LIMB_REAR_RIGHT].current_pos.x = 150;
    limbs[LIMB_REAR_RIGHT].current_pos.y = -50;
    limbs[LIMB_REAR_RIGHT].current_pos.z = 0;
   
    limbs[LIMB_REAR_RIGHT].links[LINK_COXA].length      = 45;
    limbs[LIMB_REAR_RIGHT].links[LINK_COXA].zero_rotate = 0/*-45*/;
    limbs[LIMB_REAR_RIGHT].links[LINK_COXA].min_angle   = -90;
    limbs[LIMB_REAR_RIGHT].links[LINK_COXA].max_angle   = 90;
    
    limbs[LIMB_REAR_RIGHT].links[LINK_FEMUR].length      = 85;
    limbs[LIMB_REAR_RIGHT].links[LINK_FEMUR].zero_rotate = 125;
    limbs[LIMB_REAR_RIGHT].links[LINK_FEMUR].min_angle   = 0;
    limbs[LIMB_REAR_RIGHT].links[LINK_FEMUR].max_angle   = 120;
    
    limbs[LIMB_REAR_RIGHT].links[LINK_TIBIA].length      = 141;
    limbs[LIMB_REAR_RIGHT].links[LINK_TIBIA].zero_rotate = 40;
    limbs[LIMB_REAR_RIGHT].links[LINK_TIBIA].min_angle   = 0;
    limbs[LIMB_REAR_RIGHT].links[LINK_TIBIA].max_angle   = 120;
    
    return true;
}





//  ***************************************************************************
/// @brief  Calculate angles
/// @param  info: limb info @ref limb_info_t
/// @return true - calculation success, false - no
//  ***************************************************************************
static bool kinematic_calculate_angles(limb_info_t* info) {

    int32_t coxa_zero_rotate_deg = info->links[LINK_COXA].zero_rotate;
    int32_t femur_zero_rotate_deg = info->links[LINK_FEMUR].zero_rotate;
    int32_t tibia_zero_rotate_deg = info->links[LINK_TIBIA].zero_rotate;
    uint32_t coxa_length = info->links[LINK_COXA].length;
    uint32_t femur_length = info->links[LINK_FEMUR].length;
    uint32_t tibia_length = info->links[LINK_TIBIA].length;
    
    //
    // Translate coordinates of destination point to relative limb begin
    //
    // Move to (X*, Y*, Z*) coordinate system - offset
    float x = info->current_pos.x - info->begin_position.x;
    float y = info->current_pos.y - info->begin_position.y;
    float z = info->current_pos.z - info->begin_position.z;

    // Move to (X*, Y*, Z*) coordinate system - rotate
    float coxa_zero_rotate_rad = DEG_TO_RAD(coxa_zero_rotate_deg);
    x = x * cos(coxa_zero_rotate_rad) + z * sin(coxa_zero_rotate_rad);
    y = y;
    z = -x * sin(coxa_zero_rotate_rad) + z * cos(coxa_zero_rotate_rad);


    //
    // Calculate COXA angle
    //
    float coxa_angle_rad = atan2(z, x);
    info->links[LINK_COXA].angle = RAD_TO_DEG(coxa_angle_rad);


    //
    // Prepare for calculation FEMUR and TIBIA angles
    //
    // Move to (X*, Y*) coordinate system (rotate on axis Y)
    x = x * cos(coxa_angle_rad) + z * sin(coxa_angle_rad);

    // Move to (X**, Y**) coordinate system (remove coxa from calculations)
    x = x - coxa_length;
    
    // Calculate angle between axis X and destination point
    float fi = atan2(y, x);

    // Calculate distance to destination point
    float d = sqrt(x * x + y * y);
    if (d > femur_length + tibia_length) {
        return false; // Point not attainable
    }
    
    
    //
    // Calculate triangle angles
    //
    float a = tibia_length;
    float b = femur_length;
    float c = d;

    float alpha = acos( (b * b + c * c - a * a) / (2 * b * c) );
    float gamma = acos( (a * a + b * b - c * c) / (2 * a * b) );


    //
    // Calculate FEMUR and TIBIA angle
    //
    info->links[LINK_FEMUR].angle = femur_zero_rotate_deg - RAD_TO_DEG(alpha) - RAD_TO_DEG(fi);
    info->links[LINK_TIBIA].angle = RAD_TO_DEG(gamma) - tibia_zero_rotate_deg;

    return true;
}
