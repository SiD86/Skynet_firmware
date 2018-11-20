//  ***************************************************************************
/// @file    limbs_driver.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include "limbs_driver.h"
#include "servo_driver.h"
#include "kinematic.h"
#include "veeprom.h"
#include "veeprom_map.h"
#include "error_handling.h"


typedef struct {
    
    uint32_t           link_servo[3];
    kinematic_config_t kinematic_config;
    
} limb_info_t;


static uint16_t    limb_control = 0;
static limb_info_t limb_info[SUPPORT_LIMB_COUNT] = {0};


static bool read_configuration(void);


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
}

//  ***************************************************************************
/// @brief  Start limb move
/// @param  limb: @ref limb_t
/// @param  x,y,z: new position
/// @return true - move stated, false - error
//  ***************************************************************************
void limbs_driver_start_move(limb_t limb, int32_t coxa, int32_t femur, int32_t tibia, uint32_t iteration_count) {
    
    if (callback_is_limbs_driver_error_set() == true) return; // Module disable 

    if (limb >= SUPPORT_LIMB_COUNT) {
        callback_set_selfdiag_error(ERROR_MODULE_LIMBS_DRIVER);
        return;
    }
    
    
    // Check limb state
    if ( (limb_control & (1 << limb)) == 0 ) {
        return; // Limb disable
    }
    
    
    /*// Calculate COXA, FEMUR and TIBIA angles
    int32_t link_angles[3] = {0};
    kinematic_prepare(&coxa, &femur, &tibia, &limb_info[limb].kinematic_config);
    if (kinematic_calculate_angles(coxa, femur, tibia, &limb_info[limb].kinematic_config, link_angles) == false) {
        callback_set_out_of_range_error(ERROR_MODULE_LIMBS_DRIVER);
        return;
    }
    
    servo_driver_start_move(limb_info[limb].link_servo[0], link_angles[0]);
    servo_driver_start_move(limb_info[limb].link_servo[1], link_angles[1]);
    servo_driver_start_move(limb_info[limb].link_servo[2], link_angles[2]);*/
    
    // Move servos to destination angles
    if (coxa != NO_MOVE)  servo_driver_start_move(limb_info[limb].link_servo[LINK_COXA],  coxa,  iteration_count);
    if (femur != NO_MOVE) servo_driver_start_move(limb_info[limb].link_servo[LINK_FEMUR], femur, iteration_count);
    if (tibia != NO_MOVE) servo_driver_start_move(limb_info[limb].link_servo[LINK_TIBIA], tibia, iteration_count);
}

//  ***************************************************************************
/// @brief  Stop limb move
/// @param  limb: @ref limb_t
/// @return none
//  ***************************************************************************
void limbs_driver_stop_move(limb_t limb) {
    
    if (callback_is_limbs_driver_error_set() == true) return;  // Module disable

    if (limb >= SUPPORT_LIMB_COUNT) {
        callback_set_selfdiag_error(ERROR_MODULE_LIMBS_DRIVER);
        return;
    }
    
    
    servo_driver_stop_move(limb_info[limb].link_servo[0]);
    servo_driver_stop_move(limb_info[limb].link_servo[1]);
    servo_driver_stop_move(limb_info[limb].link_servo[2]);
}

//  ***************************************************************************
/// @brief  Check all limbs move complete
/// @param  none
/// @return true - move complete, false - move in progress
//  ***************************************************************************
bool limbs_driver_is_move_complete(void) {
    
    return servo_driver_is_move_complete();
}



//  ***************************************************************************
/// @brief  Read configuration
/// @param  none
/// @return true - read success, false - fail
//  ***************************************************************************
static bool read_configuration(void) {
    
    //
    // Read limbs configuration
    //
    limb_control = veeprom_read_32(LIMB_CONTROL_EE_ADDRESS);
    for (uint32_t i = 0; i < SUPPORT_LIMB_COUNT; ++i) {
        
        if ( (limb_control & (1 << i)) == 0 ) {
            continue;; // Limb disable
        }
        
        uint32_t base_address = i * LIMB_CONFIGURATION_SIZE;
        
        // Read servo index
        uint8_t link_0 = veeprom_read_8(base_address + LIMB_0_0_SERVO_EE_ADDRESS);
        uint8_t link_1 = veeprom_read_8(base_address + LIMB_0_1_SERVO_EE_ADDRESS);
        uint8_t link_2 = veeprom_read_8(base_address + LIMB_0_2_SERVO_EE_ADDRESS);
        if (link_0 > SUPPORT_SERVO_COUNT || link_1 > SUPPORT_SERVO_COUNT || link_2 > SUPPORT_SERVO_COUNT) {
            return false;
        }
        
        limb_info[i].link_servo[0] = link_0;
        limb_info[i].link_servo[1] = link_1;
        limb_info[i].link_servo[2] = link_2;
    }
    
    //
    // Read kinematic configuration
    //
    limb_info[0].kinematic_config.coxa_begin_x = 0;
    limb_info[0].kinematic_config.coxa_begin_y = -7;
    limb_info[0].kinematic_config.coxa_begin_z = 0;
    limb_info[0].kinematic_config.link_length[LINK_COXA]  = 45;
    limb_info[0].kinematic_config.link_length[LINK_FEMUR] = 85;
    limb_info[0].kinematic_config.link_length[LINK_TIBIA] = 141;
    limb_info[0].kinematic_config.link_zero_rotate[LINK_COXA]  = 45;
    limb_info[0].kinematic_config.link_zero_rotate[LINK_FEMUR] = 125;
    limb_info[0].kinematic_config.link_zero_rotate[LINK_TIBIA] = 40;
    limb_info[0].kinematic_config.link_min_angle[LINK_COXA]  = -90;
    limb_info[0].kinematic_config.link_min_angle[LINK_FEMUR] = 0;
    limb_info[0].kinematic_config.link_min_angle[LINK_TIBIA] = 0;
    limb_info[0].kinematic_config.link_max_angle[LINK_COXA]  = 90;
    limb_info[0].kinematic_config.link_max_angle[LINK_FEMUR] = 120;
    limb_info[0].kinematic_config.link_max_angle[LINK_TIBIA] = 120;
    
    limb_info[1].kinematic_config.coxa_begin_x = 0;
    limb_info[1].kinematic_config.coxa_begin_y = -7;
    limb_info[1].kinematic_config.coxa_begin_z = 0;
    limb_info[1].kinematic_config.link_length[LINK_COXA]  = 45;
    limb_info[1].kinematic_config.link_length[LINK_FEMUR] = 85;
    limb_info[1].kinematic_config.link_length[LINK_TIBIA] = 141;
    limb_info[1].kinematic_config.link_zero_rotate[LINK_COXA]  = 0;
    limb_info[1].kinematic_config.link_zero_rotate[LINK_FEMUR] = 125;
    limb_info[1].kinematic_config.link_zero_rotate[LINK_TIBIA] = 40;
    limb_info[1].kinematic_config.link_min_angle[LINK_COXA]  = -90;
    limb_info[1].kinematic_config.link_min_angle[LINK_FEMUR] = 0;
    limb_info[1].kinematic_config.link_min_angle[LINK_TIBIA] = 0;
    limb_info[1].kinematic_config.link_max_angle[LINK_COXA]  = 90;
    limb_info[1].kinematic_config.link_max_angle[LINK_FEMUR] = 120;
    limb_info[1].kinematic_config.link_max_angle[LINK_TIBIA] = 120;
    
    limb_info[2].kinematic_config.coxa_begin_x = 0;
    limb_info[2].kinematic_config.coxa_begin_y = -7;
    limb_info[2].kinematic_config.coxa_begin_z = 0;
    limb_info[2].kinematic_config.link_length[LINK_COXA]  = 45;
    limb_info[2].kinematic_config.link_length[LINK_FEMUR] = 85;
    limb_info[2].kinematic_config.link_length[LINK_TIBIA] = 141;
    limb_info[2].kinematic_config.link_zero_rotate[LINK_COXA]  = -45;
    limb_info[2].kinematic_config.link_zero_rotate[LINK_FEMUR] = 125;
    limb_info[2].kinematic_config.link_zero_rotate[LINK_TIBIA] = 40;
    limb_info[2].kinematic_config.link_min_angle[LINK_COXA]  = -90;
    limb_info[2].kinematic_config.link_min_angle[LINK_FEMUR] = 0;
    limb_info[2].kinematic_config.link_min_angle[LINK_TIBIA] = 0;
    limb_info[2].kinematic_config.link_max_angle[LINK_COXA]  = 90;
    limb_info[2].kinematic_config.link_max_angle[LINK_FEMUR] = 120;
    limb_info[2].kinematic_config.link_max_angle[LINK_TIBIA] = 120;
    
    
    limb_info[3].kinematic_config.coxa_begin_x = 0;
    limb_info[3].kinematic_config.coxa_begin_y = -7;
    limb_info[3].kinematic_config.coxa_begin_z = 0;
    limb_info[3].kinematic_config.link_length[LINK_COXA]  = 45;
    limb_info[3].kinematic_config.link_length[LINK_FEMUR] = 85;
    limb_info[3].kinematic_config.link_length[LINK_TIBIA] = 141;
    limb_info[3].kinematic_config.link_zero_rotate[LINK_COXA]  = 45;
    limb_info[3].kinematic_config.link_zero_rotate[LINK_FEMUR] = 125;
    limb_info[3].kinematic_config.link_zero_rotate[LINK_TIBIA] = 40;
    limb_info[3].kinematic_config.link_min_angle[LINK_COXA]  = -90;
    limb_info[3].kinematic_config.link_min_angle[LINK_FEMUR] = 0;
    limb_info[3].kinematic_config.link_min_angle[LINK_TIBIA] = 0;
    limb_info[3].kinematic_config.link_max_angle[LINK_COXA]  = 90;
    limb_info[3].kinematic_config.link_max_angle[LINK_FEMUR] = 120;
    limb_info[3].kinematic_config.link_max_angle[LINK_TIBIA] = 120;
    
    limb_info[4].kinematic_config.coxa_begin_x = 0;
    limb_info[4].kinematic_config.coxa_begin_y = -7;
    limb_info[4].kinematic_config.coxa_begin_z = 0;
    limb_info[4].kinematic_config.link_length[LINK_COXA]  = 45;
    limb_info[4].kinematic_config.link_length[LINK_FEMUR] = 85;
    limb_info[4].kinematic_config.link_length[LINK_TIBIA] = 141;
    limb_info[4].kinematic_config.link_zero_rotate[LINK_COXA]  = 0;
    limb_info[4].kinematic_config.link_zero_rotate[LINK_FEMUR] = 125;
    limb_info[4].kinematic_config.link_zero_rotate[LINK_TIBIA] = 40;
    limb_info[4].kinematic_config.link_min_angle[LINK_COXA]  = -90;
    limb_info[4].kinematic_config.link_min_angle[LINK_FEMUR] = 0;
    limb_info[4].kinematic_config.link_min_angle[LINK_TIBIA] = 0;
    limb_info[4].kinematic_config.link_max_angle[LINK_COXA]  = 90;
    limb_info[4].kinematic_config.link_max_angle[LINK_FEMUR] = 120;
    limb_info[4].kinematic_config.link_max_angle[LINK_TIBIA] = 120;
    
    limb_info[5].kinematic_config.coxa_begin_x = 0;
    limb_info[5].kinematic_config.coxa_begin_y = -7;
    limb_info[5].kinematic_config.coxa_begin_z = 0;
    limb_info[5].kinematic_config.link_length[LINK_COXA]  = 45;
    limb_info[5].kinematic_config.link_length[LINK_FEMUR] = 85;
    limb_info[5].kinematic_config.link_length[LINK_TIBIA] = 141;
    limb_info[5].kinematic_config.link_zero_rotate[LINK_COXA]  = -45;
    limb_info[5].kinematic_config.link_zero_rotate[LINK_FEMUR] = 125;
    limb_info[5].kinematic_config.link_zero_rotate[LINK_TIBIA] = 40;
    limb_info[5].kinematic_config.link_min_angle[LINK_COXA]  = -90;
    limb_info[5].kinematic_config.link_min_angle[LINK_FEMUR] = 0;
    limb_info[5].kinematic_config.link_min_angle[LINK_TIBIA] = 0;
    limb_info[5].kinematic_config.link_max_angle[LINK_COXA]  = 90;
    limb_info[5].kinematic_config.link_max_angle[LINK_FEMUR] = 120;
    limb_info[5].kinematic_config.link_max_angle[LINK_TIBIA] = 120;
    
    return true;
}