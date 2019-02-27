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

uint32_t iter_count = 15;

#define TOTAL_ITERATION_COUNT		   (iter_count)
#define DELAY_BETWEEN_ITERATIONS       (10)


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

	path_type_t path_type;
	point_3d_t  start_point;
	point_3d_t  dest_point;

} path_3d_t;

typedef struct {
    
    point_3d_t position;
	path_3d_t  movement_path;
	
    link_info_t links[3];
    
} limb_info_t;


static driver_state_t driver_state = STATE_NOINIT;
static limb_info_t    limbs[SUPPORT_LIMB_COUNT] = {0};
static uint32_t		  current_movement_iteration = 0;


static bool read_configuration(void);
static void path_calculate_point(const path_3d_t* info, uint32_t current_iteration, point_3d_t* point);
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
    for (uint32_t i = 0; i < SUPPORT_LIMB_COUNT; ++i) {
        servo_driver_move(i * 3 + 0, limbs[i].links[LINK_COXA].angle);
        servo_driver_move(i * 3 + 1, limbs[i].links[LINK_FEMUR].angle);
        servo_driver_move(i * 3 + 2, limbs[i].links[LINK_TIBIA].angle);
    }
    
    // Initialization driver state
    driver_state = STATE_IDLE;
}

//  ***************************************************************************
/// @brief  Start limb move
/// @param  point_list: destination point list
/// @param  path_type_list: path type list
//  ***************************************************************************
void limbs_driver_start_move(const point_3d_t* point_list, const path_type_t* path_type_list) {
    
    if (point_list == NULL) {
        callback_set_internal_error(ERROR_MODULE_LIMBS_DRIVER);
        return;
    }
    
    // Prepare limbs for movement
    for (uint32_t i = 0; i < SUPPORT_LIMB_COUNT; ++i) {
        
        // Prepare limb for movement
		limbs[i].movement_path.path_type   = path_type_list[i];
        limbs[i].movement_path.start_point = limbs[i].position;
		limbs[i].movement_path.dest_point  = point_list[i];
        
        // Need start movement?
        if (limbs[i].position.x == point_list[i].x && limbs[i].position.y == point_list[i].y && limbs[i].position.z == point_list[i].z) {
            continue;
        }
        
		current_movement_iteration = 0;
        driver_state = STATE_CALC;
    }
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
            if (current_movement_iteration > TOTAL_ITERATION_COUNT) {
				driver_state = STATE_IDLE;
				break;
			}
            for (uint32_t i = 0; i < SUPPORT_LIMB_COUNT; ++i) {
				path_calculate_point(&limbs[i].movement_path, current_movement_iteration, &limbs[i].position);
                kinematic_calculate_angles(&limbs[i]);
            }
			
			++current_movement_iteration;
			driver_state = STATE_LOAD;
            break;
            
        case STATE_LOAD:
            for (uint32_t i = 0; i < SUPPORT_LIMB_COUNT; ++i) {
                servo_driver_move(i * 3 + 0, limbs[i].links[LINK_COXA].angle);
                servo_driver_move(i * 3 + 1, limbs[i].links[LINK_FEMUR].angle);
                servo_driver_move(i * 3 + 2, limbs[i].links[LINK_TIBIA].angle);
            }
            
            begin_wait_time = get_time_ms();
            driver_state = STATE_WAIT;
            break;

        case STATE_WAIT:
            if (get_time_ms() - begin_wait_time >= DELAY_BETWEEN_ITERATIONS) {
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
    
	for (uint32_t i = 0; i < SUPPORT_LIMB_COUNT; ++i) {
		
		uint32_t base_address = i * LIMB_CONFIGURATION_SIZE;
		
		// Read coxa, femur and tibia lengths
		limbs[i].links[LINK_COXA].length = (uint16_t)veeprom_read_16(base_address + LIMB_COXA_LENGTH_EE_ADDRESS);
		if (limbs[i].links[LINK_COXA].length == 0xFFFF) {
			return false;
		}
		limbs[i].links[LINK_FEMUR].length = (uint16_t)veeprom_read_16(base_address + LIMB_FEMUR_LENGTH_EE_ADDRESS);
		if (limbs[i].links[LINK_FEMUR].length == 0xFFFF) {
			return false;
		}
		limbs[i].links[LINK_TIBIA].length = (uint16_t)veeprom_read_16(base_address + LIMB_TIBIA_LENGTH_EE_ADDRESS);
		if (limbs[i].links[LINK_TIBIA].length == 0xFFFF) {
			return false;
		}
		
		// Read coxa, femur and tibia zero rotate
		limbs[i].links[LINK_COXA].zero_rotate = (int16_t)veeprom_read_16(base_address + LIMB_COXA_ZERO_ROTATE_EE_ADDRESS);
		if (limbs[i].links[LINK_COXA].zero_rotate < -360 || limbs[i].links[LINK_COXA].zero_rotate > 360) {
			return false;
		}
		limbs[i].links[LINK_FEMUR].zero_rotate = (int16_t)veeprom_read_16(base_address + LIMB_FEMUR_ZERO_ROTATE_EE_ADDRESS);
		if (limbs[i].links[LINK_FEMUR].zero_rotate < -360 || limbs[i].links[LINK_FEMUR].zero_rotate > 360) {
			return false;
		}
		limbs[i].links[LINK_TIBIA].zero_rotate = (int16_t)veeprom_read_16(base_address + LIMB_TIBIA_ZERO_ROTATE_EE_ADDRESS);
		if (limbs[i].links[LINK_TIBIA].zero_rotate < -360 || limbs[i].links[LINK_TIBIA].zero_rotate > 360) {
			return false;
		}
		
		// Read coxa, femur and tibia angle ranges
		limbs[i].links[LINK_COXA].min_angle  = (int8_t)veeprom_read_8(base_address + LIMB_COXA_MIN_ANGLE_EE_ADDRESS);
		limbs[i].links[LINK_COXA].max_angle  = (int8_t)veeprom_read_8(base_address + LIMB_COXA_MAX_ANGLE_EE_ADDRESS);
		limbs[i].links[LINK_FEMUR].min_angle = (int8_t)veeprom_read_8(base_address + LIMB_FEMUR_MIN_ANGLE_EE_ADDRESS);
		limbs[i].links[LINK_FEMUR].max_angle = (int8_t)veeprom_read_8(base_address + LIMB_FEMUR_MAX_ANGLE_EE_ADDRESS);
		limbs[i].links[LINK_TIBIA].min_angle = (int8_t)veeprom_read_8(base_address + LIMB_TIBIA_MIN_ANGLE_EE_ADDRESS);
		limbs[i].links[LINK_TIBIA].max_angle = (int8_t)veeprom_read_8(base_address + LIMB_TIBIA_MAX_ANGLE_EE_ADDRESS);
		
		// Read coxa, femur and tibia start position
		limbs[i].position.x = (int16_t)veeprom_read_16(base_address + LIMB_START_POSITION_X_EE_ADDRESS);
		limbs[i].position.y = (int16_t)veeprom_read_16(base_address + LIMB_START_POSITION_Y_EE_ADDRESS);
		limbs[i].position.z = (int16_t)veeprom_read_16(base_address + LIMB_START_POSITION_Z_EE_ADDRESS);
	}

    return true;
}

//  ***************************************************************************
/// @brief  Calculate path point
/// @param  info: path info @ref path_3d_t
/// @param  current_iteration: current iteration index [0; TOTAL_ITERATION_COUNT]
/// @param  point: calculated point
/// @retval point
//  ***************************************************************************
static void path_calculate_point(const path_3d_t* info, uint32_t current_iteration, point_3d_t* point) {

	float t_max = RAD_TO_DEG(M_PI); // [0; Pi]
	float t = current_iteration * (t_max / TOTAL_ITERATION_COUNT); // iter_index * dt

	float x0 = info->start_point.x;
	float y0 = info->start_point.y;
	float z0 = info->start_point.z;
	float x1 = info->dest_point.x;
	float y1 = info->dest_point.y;
	float z1 = info->dest_point.z;


	if (info->path_type == PATH_LINEAR) {
    	point->x = t * (x1 - x0) / t_max + x0;
    	point->y = t * (y1 - y0) / t_max + y0;
    	point->z = t * (z1 - z0) / t_max + z0;
	}

	if (info->path_type == PATH_XZ_ARC_Y_LINEAR) {

    	float R = sqrt(x0 * x0 + z0 * z0);
    	float atan0 = RAD_TO_DEG(atan2(x0, z0));
    	float atan1 = RAD_TO_DEG(atan2(x1, z1));

    	float t_mapped_rad = DEG_TO_RAD(t * (atan1 - atan0) / t_max + atan0);
    	point->x = R * sin(t_mapped_rad); // Circle Y
    	point->y = t * (info->dest_point.y - info->start_point.y) / t_max + info->start_point.y;
    	point->z = R * cos(t_mapped_rad); // Circle X
	}
	
	if (info->path_type == PATH_XZ_ARC_Y_SINUS) {
		
		float R = sqrt(x0 * x0 + z0 * z0);
		float atan0 = RAD_TO_DEG(atan2(x0, z0));
		float atan1 = RAD_TO_DEG(atan2(x1, z1));

		float t_mapped_rad = DEG_TO_RAD(t * (atan1 - atan0) / t_max + atan0);

		point->x = R * sin(t_mapped_rad); // circle Y
		point->y = (y1 - y0) * sin(DEG_TO_RAD(t)) + y0;
		point->z = R * cos(t_mapped_rad); // circle X
	}
	
	if (info->path_type == PATH_XZ_ELLIPTICAL_Y_SINUS) {

		float a = (z1 - z0) / 2.0f;
		float b = (x1 - x0);
		float c = (y1 - y0);

		point->x = b * sin(DEG_TO_RAD(t_max - t)) + x0; // circle Y
		point->y = c * sin(DEG_TO_RAD(t)) + y0;
		point->z = a * cos(DEG_TO_RAD(t_max - t)) + z0 + a;
	}
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
	
	float x = info->position.x;
	float y = info->position.y;
    float z = info->position.z;
	
	
    // Move to (X*, Y*, Z*) coordinate system - rotate
    float coxa_zero_rotate_rad = DEG_TO_RAD(coxa_zero_rotate_deg);
    float x1 = x * cos(coxa_zero_rotate_rad) + z * sin(coxa_zero_rotate_rad);
    float y1 = y;
    float z1 = -x * sin(coxa_zero_rotate_rad) + z * cos(coxa_zero_rotate_rad);


    //
    // Calculate COXA angle
    //
    float coxa_angle_rad = atan2(z1, x1);
    info->links[LINK_COXA].angle = RAD_TO_DEG(coxa_angle_rad);


    //
    // Prepare for calculation FEMUR and TIBIA angles
    //
    // Move to (X*, Y*) coordinate system (rotate on axis Y)
    x1 = x1 * cos(coxa_angle_rad) + z1 * sin(coxa_angle_rad);

    // Move to (X**, Y**) coordinate system (remove coxa from calculations)
    x1 = x1 - coxa_length;
    
    // Calculate angle between axis X and destination point
    float fi = atan2(y1, x1);

    // Calculate distance to destination point
    float d = sqrt(x1 * x1 + y1 * y1);
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
	
	//
	// Check angles
	//
	if (info->links[LINK_COXA].angle < info->links[LINK_COXA].min_angle || info->links[LINK_COXA].angle > info->links[LINK_COXA].max_angle) {
		return false;
	}
	if (info->links[LINK_FEMUR].angle < info->links[LINK_FEMUR].min_angle || info->links[LINK_FEMUR].angle > info->links[LINK_FEMUR].max_angle) {
		return false;
	}
	if (info->links[LINK_TIBIA].angle < info->links[LINK_TIBIA].min_angle || info->links[LINK_TIBIA].angle > info->links[LINK_TIBIA].max_angle) {
		return false;
	}

    return true;
}
